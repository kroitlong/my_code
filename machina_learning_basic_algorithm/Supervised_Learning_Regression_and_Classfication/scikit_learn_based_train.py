from sklearn.linear_model import SGDRegressor
from sklearn.preprocessing import StandardScaler
import numpy as np


# 使用sklearn库进行训练

def prepare_date(filename):
    _x = []
    x_list = []
    y_list = []
    with open('./' + filename, 'r') as file:
        for line in file:
            # 使用逗号分割数据
            parts = line.split(',')
            for i in range(len(parts) - 1):
                _x.append(float(parts[i].strip()))
            # 将分割后的数据添加到不同的列表
            x_list.append(_x)
            y_list.append(float(parts[-1].strip()))
            _x = []
    del _x
    return x_list, y_list


x_train, y_train = prepare_date('houses.txt')
# 数据标准化过程
scalar = StandardScaler()
print(f"before fit_transform:{np.ptp(np.array(x_train), axis=0)}")
x_norm = scalar.fit_transform(x_train)
print(f"after fit_transform:{np.ptp(np.array(x_norm), axis=0)}")

# 创建一个随机线性回归模型
sgdr = SGDRegressor(max_iter=1000, tol=1.0e-3)
sgdr.fit(x_norm, y_train)
print(f"iter_times:{sgdr.n_iter_}")

# 获取训练后的参数列表
b_get = sgdr.intercept_
w_get = sgdr.coef_
print(f"model param:    w:{w_get},b:{b_get}")

# 进行预测
y_pred = sgdr.predict(x_norm[:4])
print(f"Prediction on training set:\n{y_pred[:4]}")
print(f"Target values \n{y_train[:4]}")
