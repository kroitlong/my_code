# 用来测试代码功能的demo，动态的变化着。
import numpy as np

vec = np.array((1, 2, 3))
print(vec)
new_vec = np.tile(vec, (4, 1))
print(new_vec)
new_vec = np.tile(vec, (4, 2))
print(new_vec)
