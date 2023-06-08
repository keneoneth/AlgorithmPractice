import hidet
from hidet.ir.compute import TensorNode, compute, reduce
from hidet.ir.expr import Expr, Call
from hidet.ir.task import Task
from hidet.ir.func import IRModule
from hidet.ir.primitives.func import register_primitive_function
from hidet.ir.type import FuncType, VoidType
from hidet.ir.primitives.func import call_primitive_func, is_primitive_function
from hidet.ir.type import DataType, PointerType, TensorPointerType

register_primitive_function(name='my_abc',func_or_type=FuncType([VoidType(),VoidType()],VoidType()),codegen_name='abc')

def abc(x,y) -> Call:
    return call_primitive_func('my_abc', [x,y])

print('abc',is_primitive_function('my_abc'))

class BatchMatmulFp16Task(Task):
    def __init__(self, a: TensorNode, b: TensorNode):
        batch_size, m_size, k_size = a.const_shape()
        batch_size, k_size, n_size = b.const_shape()
        c = compute(
            name='c',
            shape=[batch_size, m_size, n_size],
            fcompute=lambda p, i, j: reduce(
                shape=[k_size],
                fcompute=lambda k: a[p, i, k] * b[p, k, j],
                reduce_type='sum',
            ),
        )
        super().__init__(
            name='batch_matmul_fp16',
            inputs=[a, b],
            outputs=[c],
            attributes={
                'batch_size': batch_size,
                'm_size': m_size,
                'n_size': n_size,
                'k_size': k_size,
            },
        )

    def allow_epilogue(self) -> bool:
        return False

    def implement_cuda(self, working_dir: str) -> IRModule:
        # override this method to use template-based scheduling
        return batch_matmul_mma_fp16_schedule(self)
    
def batch_matmul_mma_fp16_schedule(task: BatchMatmulFp16Task) -> IRModule:
    from hidet.lang import f16, spatial, repeat, tensor, attr, grid, printf, cast, tensor_pointer
    from hidet.lang.mapping import repeat, spatial
    from hidet.lang.cuda import blockIdx, threadIdx, syncthreads
    from hidet.lang.cuda import MmaConfig, mma_sync

    # get the workload size
    bs = task.attrs['batch_size']
    m_size = task.attrs['m_size']
    n_size = task.attrs['n_size']
    k_size = task.attrs['k_size']

    # define the template hyper-parameters
    mma_config = MmaConfig.m16n8k8_f16_f16()
    block_m, block_n, block_k = 128, 128, 8
    warp_m, warp_n, warp_k = 64, 64, 8
    warp_count_m, warp_count_n, warp_count_k = 2, 2, 1
    mma_m, mma_n, mma_k = mma_config.m, mma_config.n, mma_config.k  # 16, 8, 8
    mma_count_m, mma_count_n, mma_count = 4, 8, 1
    threads = warp_count_m * warp_count_n * warp_count_k * 32

    # define the tensor program
    with hidet.script_module() as module:

        @hidet.script
        def load_regs_a(
            smem_a: f16[block_m, block_k], regs_a: f16[4, mma_config.a_elements]
        ):
            """Load A registers from shared memory."""
            warp_id, lane_id = threadIdx.x / 32, threadIdx.x % 32
            for wi, wj, wk in spatial(warp_count_m, warp_count_n, warp_count_k).on(
                warp_id
            ):
                for mi in range(mma_count_m):
                    p = 0
                    for i, k in mma_config.a_load_map.on(lane_id):
                        regs_a[mi, p] = smem_a[
                            wi * warp_m + mi * mma_m + i, wk * warp_k + k
                        ]
                        p += 1

        @hidet.script
        def load_regs_b(
            smem_b: f16[block_k, block_n], regs_b: f16[8, mma_config.b_elements]
        ):
            """Load B registers from shared memory."""
            warp_id, lane_id = threadIdx.x / 32, threadIdx.x % 32
            for wi, wj, wk in spatial(warp_count_m, warp_count_n, warp_count_k).on(
                warp_id
            ):
                for mj in range(mma_count_n):
                    p = 0
                    for k, j in mma_config.b_load_map.on(lane_id):
                        regs_b[mj, p] = smem_b[
                            wk * warp_k + k, wj * warp_n + mj * mma_n + j
                        ]
                        p += 1

        @hidet.script
        def warp_mma(
            regs_a: f16[4, mma_config.a_elements],
            regs_b: f16[8, mma_config.b_elements],
            regs_c: f16[4, 8, mma_config.c_elements],
        ):
            """Perform warp-level matrix multiplication."""
            for mi, mj in repeat(mma_count_m, mma_count_n).on(0):
                mma_sync(mma_config, ~regs_a[mi, 0], ~regs_b[mj, 0], ~regs_c[mi, mj, 0])

        @hidet.script
        def store_c(regs_c: f16[4, 8, mma_config.c_elements], c: f16[bs, m_size, n_size]):
            """Store C registers to global memory."""
            warp_id, lane_id = threadIdx.x / 32, threadIdx.x % 32
            offset_m, offset_n = blockIdx.x * block_m, blockIdx.y * block_n
            gmem_c = c[blockIdx.z, offset_m:, offset_n:]
            for k_round in range(warp_count_k):
                for wi, wj, wk in spatial(warp_count_m, warp_count_n, warp_count_k).on(
                    warp_id
                ):
                    if wk == k_round:
                        for mi, mj in repeat(mma_count_m, mma_count_n).on(0):
                            p = 0
                            for i, j in mma_config.c_store_map.on(lane_id):
                                gmem_c.write(
                                    [
                                        wi * warp_m + mi * mma_m + i,
                                        wj * warp_n + mj * mma_n + j,
                                    ],
                                    regs_c[mi, mj, p],
                                    protected=True,
                                )
                                p += 1

        @hidet.script
        def batch_matmul_kernel(
            a: f16[bs, m_size, k_size],
            b: f16[bs, k_size, n_size],
            c: f16[bs, m_size, n_size],
        ):
            """Batch matrix multiplication kernel."""
            attr.cuda_grid_dim = (
                (m_size + block_m - 1) // block_m,
                (n_size + block_n - 1) // block_n,
                bs,
            )
            attr.cuda_block_dim = threads
            offset_m, offset_n = blockIdx.x * block_m, blockIdx.y * block_n
            smem_a = tensor('shared', 'float16', [block_m, block_k])
            smem_b = tensor('shared', 'float16', [block_k, block_n])
            regs_a = tensor('register', 'float16', [4, mma_config.a_elements])
            regs_b = tensor('register', 'float16', [8, mma_config.b_elements])
            regs_c = tensor('register', 'float16', [4, 8, mma_config.c_elements])

            for i, j, p in grid(4, 8, mma_config.c_elements):
                regs_c[i, j, p] = 0.0

            for k0 in range((k_size + block_k - 1) // block_k):
                offset_k = k0 * block_k
                gmem_a = a[blockIdx.z, offset_m:, offset_k:]
                gmem_b = b[blockIdx.z, offset_k:, offset_n:]
                for i, k in repeat(8, 1).spatial(16, 8).on(threadIdx.x):
                    smem_a[i, k] = gmem_a.read([i, k], protected=True)
                for k, j in repeat(8, 1).spatial(1, 128).on(threadIdx.x):
                    smem_b[k, j] = gmem_b.read([k, j], protected=True)
                syncthreads()
                # smem_b = myabc(smem_a)
                # "__ubuf__ float * hihihi;"
                trya = tensor_pointer('float16',shape=[2])
                abc(smem_a,trya)
                load_regs_a(smem_a, regs_a)
                load_regs_b(smem_b, regs_b)
                warp_mma(regs_a, regs_b, regs_c)

                syncthreads()
            store_c(regs_c, c)

    ir_module = module.ir_module()
    return ir_module


    

from hidet.graph import Operator, Tensor
from hidet.graph.ops.definitions.utils import input_like


class BatchMatmulFp16Op(Operator):
    def __init__(self, a: Tensor, b: Tensor):
        assert a.dtype == hidet.float16 and b.dtype == hidet.float16
        super().__init__(
            inputs=[a, b],
            attributes={},
            task=BatchMatmulFp16Task(input_like(a, 'a'), input_like(b, 'b')),
        )


def batch_matmul_fp16(a: Tensor, b: Tensor) -> Tensor:
    return BatchMatmulFp16Op(a, b).get_output(0)


def demo_usage():
    a = hidet.randn([1, 2, 2], dtype='float16', device='cuda')
    b = hidet.randn([1, 2, 2], dtype='float16', device='cuda')
    c = batch_matmul_fp16(a, b)
    print(a)
    print(b)
    print(c)


# demo_usage()


# %%
# Generated Source Code
# ---------------------
# If you are interested in the generated source code, here it is:

# sphinx_gallery_start_ignore
# a = hidet.randn([1, 2, 2], dtype='float16', device='cuda')
# b = hidet.randn([1, 2, 2], dtype='float16', device='cuda')

a = hidet.randn([1, 2, 2], dtype='float16', device='cuda')
b = hidet.randn([1, 2, 2], dtype='float16', device='cuda')
src_path = BatchMatmulFp16Task(input_like(a, 'a'), input_like(b, 'b')).build(target='cuda')
print('ret',src_path)

# op = BatchMatmulFp16Op(a, b)

# from hidet.driver import build_task
# build_task(op)
# print('??',type(op))


# print('??',op.task_func.src_path)
# from hidet.lang.cuda import MmaConfig, mma_sync
# from hidet.lang.cuda import blockIdx, threadIdx, syncthreads
# print('sync',type(syncthreads),syncthreads)
# print('mma_sync',type(mma_sync),mma_sync)
# # print('myabc',type(myabc),myabc)

# # c = op.get_output(0)
# func = op.task_func
# import os

# relative_path = os.path.relpath(
#     func.src_path, os.path.dirname(hidet.utils.hidet_cache_dir())
# )

source_path = src_path
# sphinx_gallery_end_ignore
import os
# we hide the code to get the source path for simplicity
# print('Generated source path (relative to hidet cache root): \n{}'.format(relative_path))
print('Generated source code:',source_path)

import shutil
shutil.copyfile(source_path,os.path.join("./",os.path.basename(source_path)))
with open(source_path, 'r') as f:
    print(f.read())



def visit_Address(self, e: Address):
    print('visit address',type(e.expr),e.expr)
    if type(e.expr) == TensorElement:
        print(e.expr,e.expr.base,type(e.expr.base),e.expr.indices,type(e.expr.indices))
        print(type(self.visit(e.expr.base)),self.visit(e.expr.base))
        print(type(self.visit(e.expr.indices)),self.visit(e.expr.indices))
        print(type(Text('&') + self.visit(e.expr)))
        print(type(Text('&')),type(self.visit(e.expr)))
        return self.visit(e.expr.base) + Text('+') + self.visit(e.expr.indices)
    else:
        return Text('&') + self.visit(e.expr)