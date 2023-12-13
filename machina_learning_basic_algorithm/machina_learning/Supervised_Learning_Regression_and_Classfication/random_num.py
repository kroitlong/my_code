import random
import numpy as np

# 获得随机的数据，可自行指定数据的分布
x = []
y = []
ans = []
j_rec = np.zeros((100,))
for i in range(100):
    x.append(random.uniform(-10.0, 10.0))
    y.append(random.uniform(-10.0, 10.0))
for i in range(100):
    for j in range(100):
        if x[i] * y[j] >= 9 and j_rec[j] != 1:
            ans.append([x[i], y[j]])
            j_rec[j] = 1
            break
    if len(ans) < 100:
        continue
    else:
        break
for i in range(len(ans)):
    ans[i].append(1)

len_=len(ans)
x = []
y = []
j_rec = np.zeros((100,))
for i in range(100):
    x.append(random.uniform(-3.0, 3.0))
    y.append(random.uniform(-3.0, 3.0))
for i in range(100):
    for j in range(100):
        if x[i] * y[j] < 9 and j_rec[j] != 1:
            ans.append([x[i], y[j]])
            j_rec[j] = 1
            break
    if len(ans)-len_ < 50:
        continue
    else:
        break
print(len(ans))
random.shuffle(ans)
for i in range(len(ans)):
    ans[i].append(0)
    print(ans[i][0], ",", ans[i][1],",",ans[i][2])

