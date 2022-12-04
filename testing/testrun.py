
import subprocess
import pandas as pd

TEST_CAND_ARR = ["A","B","C","D","E","F","G"]
TEST_TYPE = ["I","II"]
TEST_ID = ["i","ii","iii","iv","v","vi"]


def parse_out_to_dict(out):
    ret = {}
    for line in out:
        if line.startswith("[TEST]"):
            d = eval(line[len("[TEST]"):])
            ret.update(d)
    if "elapsed_time_us" in ret:
        ret["elapsed_time_ms"] = [ret["elapsed_time_us"][0]/1000]
    return pd.DataFrame(ret)

def test_all():
    func_name = "test_all"
    report_name = f"{func_name}_result.csv"
    out_arr = []
    for tcand in TEST_CAND_ARR:
        for ttype in TEST_TYPE:
            for tid in TEST_ID:
                cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
                print(f"cmd: {cmd}")
                out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
                out = parse_out_to_dict(out)
                print(f"{out}")
                out_arr.append(out)
    result = pd.concat(out_arr)
    result.to_csv(report_name,index=False)
    print(f"test result is written to {report_name}")
    print(f"{func_name} done !!!")

def test_i():
    func_name = "test_i"
    report_name = f"{func_name}_result.csv"
    out_arr = []
    for tcand in TEST_CAND_ARR:
        for ttype in TEST_TYPE:
            tid = TEST_ID[0]
            cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
            print(f"cmd: {cmd}")
            try:
                out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
                out = parse_out_to_dict(out)
            except:
                out = parse_out_to_dict({})
            print(f"{out}")
            out_arr.append(out)
    result = pd.concat(out_arr)
    result.to_csv(report_name,index=False)
    print(f"test result is written to {report_name}")
    print(f"{func_name} done !!!")

def test_i2iii():
    func_name = "test_i2iii"
    report_name = f"{func_name}_result.csv"
    out_arr = []
    for tcand in TEST_CAND_ARR:
        for ttype in TEST_TYPE:
            for tid in TEST_ID[0:3]:
                cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
                print(f"cmd: {cmd}")
                try:
                    out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
                    out = parse_out_to_dict(out)
                except:
                    out = parse_out_to_dict({})
                print(f"{out}")
                out_arr.append(out)
    result = pd.concat(out_arr)
    result.to_csv(report_name,index=False)
    print(f"test result is written to {report_name}")
    print(f"{func_name} done !!!")

def test_simple_I():
    func_name = "test_simple_I"
    report_name = f"{func_name}_result.csv"
    out_arr = []
    for tcand in TEST_CAND_ARR[0:3]:
        for ttype in TEST_TYPE[0:1]:
            for tid in TEST_ID[0:5]:
                cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
                print(f"cmd: {cmd}")
                try:
                    out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
                    out = parse_out_to_dict(out)
                except:
                    out = parse_out_to_dict({})
                print(f"{out}")
                out_arr.append(out)
    result = pd.concat(out_arr)
    result.to_csv(report_name,index=False)
    print(f"test result is written to {report_name}")
    print(f"{func_name} done !!!")

def test_simple_II():
    func_name = "test_simple_II"
    report_name = f"{func_name}_result.csv"
    out_arr = []
    for tcand in TEST_CAND_ARR[0:3]:
        for ttype in TEST_TYPE[1:2]:
            for tid in TEST_ID[0:2]:
                cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
                print(f"cmd: {cmd}")
                try:
                    out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
                    out = parse_out_to_dict(out)
                except:
                    out = parse_out_to_dict({})
                print(f"{out}")
                out_arr.append(out)
    result = pd.concat(out_arr)
    result.to_csv(report_name,index=False)
    print(f"test result is written to {report_name}")
    print(f"{func_name} done !!!")

def test_single():
    func_name = "test_single"
    report_name = f"{func_name}_result.csv"
    tcand = TEST_CAND_ARR[1]
    ttype = TEST_TYPE[1]
    tid = TEST_ID[1]
    cmd = f"make clean && make run cand={tcand} type={ttype} id={tid}"
    print(f"cmd: {cmd}")
    try:
        out = subprocess.check_output(cmd,shell=True).decode('utf-8').split('\n')
        out = parse_out_to_dict(out)
    except:
        out = parse_out_to_dict({})
    out.to_csv(report_name,index=False)
    print(f"{out}")
    print(f"{func_name} done !!!")
    


if __name__ == "__main__":
    test_single()