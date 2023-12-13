import numpy as np
import math
import copy


# 进行逻辑回归，分为导入数据，计算logistic函数，计算损失值，计算梯度，进行梯度下降

# 导入数据
def import_data(file):
    """
    导入数据
    :param file:数据文件的名字
    :return: x_train,y_train
    """
    _x = []
    x_list = []
    y_list = []
    with open('./' + file, 'r') as file:
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


# 计算logistic函数
def sigmod(x, w, b):
    """
    计算logistic函数
    :param x: x_train,训练的数据集
    :param w: 参数列表
    :param b: 截距
    :return: 计算逻辑函数:1/(1+e^(-(w.x+b)))的值
    """
    tmp = 1 + np.exp(-1 * (np.dot(x, w) + b))
    return 1 / tmp


# 计算损失值
def compute_cost(x, y, w, b):
    """
    计算损失值
    :param x: x_train,训练的数据集
    :param y: y_train,训练的数据集
    :param w: 参数列表
    :param b: 截距
    :return: 损失值:-1/m*sum(i,m){y[i]log(fwb)+(1-y[i])log(1-fwb)}
    """
    m = x.shape[0]
    cost = 0.
    for i in range(m):
        cost += y[i] * np.log(sigmod(x[i], w, b)) + (1 - y[i]) * np.log(
            1 - sigmod(x[i], w, b))
    cost /= m
    return -1 * cost


# 计算梯度
def compute_grad(x, y, w, b):
    """
    计算梯度
    :param x: x_train,训练的数据集
    :param y: y_train,训练的数据集
    :param w: 参数列表
    :param b: 截距
    :return: 当前损失函数的梯度值列表 w_grad,b_grad
    """
    m, n = x.shape
    w_grad = np.zeros((n,))
    b_grad = 0.
    for i in range(m):
        tmp = sigmod(x[i], w, b) - y[i]
        for j in range(n):
            w_grad[j] += tmp * x[i, j]
        b_grad += tmp
    w_grad /= m
    b_grad /= m
    return w_grad, b_grad


# 进行梯度下降
def grad_descent(x_, y_, w_, b_, iter_times, learning_rate):
    """
    进行梯度下降
    :param x_:训练数据
    :param y_: 标签
    :param w_: 参数列表
    :param b_: 截距
    :param iter_times:迭代次数
    :param learning_rate: 学习率
    :return: 训练后模型参数列表以及截距和损失值列表
    """
    w = copy.deepcopy(w_)
    b = b_
    cost_history = []
    for i in range(iter_times):
        w_grad, b_grad = compute_grad(x_, y_, w, b)
        w -= learning_rate * w_grad
        b -= learning_rate * b_grad
        if i < 1000000 and i % math.ceil(iter_times / 10) == 0:
            cost_history.append(compute_cost(x_, y_, w, b))
            print(f"迭代次数:{i}，当前损失值:{cost_history[-1]}")
    return w, b, cost_history


# 执行训练
x_train, y_train = import_data("logistic_data.txt")
learning_rate = 3.0e-3
num_iters = 10000

# 这里拟合一个二次曲线,再处理一下x_train
m = len(x_train)
for i in range(m):
    x_train[i].append(pow(x_train[i][0], 2))
    x_train[i].append(pow(x_train[i][1], 2))
    x_train[i].append(x_train[i][0] * x_train[i][1])
x_train = np.array(x_train)
y_train = np.array(y_train)
w = np.zeros((x_train.shape[1],))
b = 0.
w_train, b_train, cost_list = grad_descent(x_train, y_train, w, b, num_iters, learning_rate)
print(f"参数:{w_train}, {b_train}")

# 进行预测
while 1:
    user_input = input("请输入整数列表（用逗号分隔元素,2个参数）: ")
    input_list = user_input.split(',')  # 以逗号分隔字符串并创建一个字符串列表
    input_list = [float(x) for x in input_list]
    input_list.append(pow(input_list[0], 2))
    input_list.append(pow(input_list[1], 2))
    input_list.append(input_list[0] * input_list[1])
    x_input_list = np.array([float(x) for x in input_list])  # 将字符串列表转换为整数列表
    ans = sigmod(x_input_list, w_train, b_train)
    print(f"\n预测结果为{ans}")
