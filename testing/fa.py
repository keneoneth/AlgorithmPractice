import torch
import torch.nn as nn
import numpy as np

sfm = nn.Softmax(dim=-1)
input = torch.from_numpy(np.array([2,4,9,3],dtype=np.float32))
output = sfm(input-max(input))

print(output.numpy())


# find max
np_input = np.array(input)
max_in = max(np_input)
in_minus_max = np_input - max_in
print(in_minus_max)
exp_form = np.exp(in_minus_max)
print(exp_form)
sum_form = np.sum(exp_form)
print(sum_form)

ans = exp_form / sum_form
print('gold ans', ans)


### split it here
inA = np_input[0:2]
inB = np_input[2:4]

print(inA,inB)

maxA = max(inA)
inA_minus_max = (inA - maxA)
expA = np.exp(inA_minus_max)
sumA = np.sum(expA)
ansA = expA / sumA
print('a',ansA)

prev_m = maxA
prev_l = sumA
prev_o = ansA

maxB = max(inB)
inB_minus_max = (inB - maxB)
expB = np.exp(inB_minus_max)
sumB = np.sum(expB)
ansB = expB / sumB
print('b',ansB)



new_m = max(maxB,maxA)

# you can find the answer here
update_m = np.exp(prev_m-new_m)
local_update_m = np.exp(maxB-new_m)
# print(update_m,new_update_m)
new_l = update_m * prev_l + local_update_m * sumB
# print(ansA*)

print((new_l**-1)*prev_l*update_m*prev_o)
print((new_l**-1)*local_update_m*ansB)


print('OK')