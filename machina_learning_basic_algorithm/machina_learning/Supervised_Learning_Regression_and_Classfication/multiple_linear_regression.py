import numpy as np
import math
import copy
import matplotlib.pyplot as plt


# 计算损失值
def compute_cost(x, y, w, b):
    m = y.shape[0]
    cost = 0
    for i in range(m):
        predict = np.dot(w, x[i]) + b
        temp = (predict - y[i]) ** 2
        cost += temp
    cost /= 2 * m
    return cost


# 计算梯度
def compute_grad(x, y, w, b):
    m = y.shape[0]
    grad_list = []
    for i in range(x[0].shape[0]):
        temp = 0
        for j in range(m):
            predict = np.dot(x[j], w) + b
            cost = predict - y[j]
            grad = cost * x[j][i]
            temp += grad
        temp /= m
        grad_list.append(temp)
    temp = 0
    for i in range(m):
        predict = np.dot(x[i], w) + b
        cost = predict - y[i]
        temp += cost
    temp /= m
    grad_list.append(temp)
    grad_list = np.array(grad_list)
    return grad_list


# 执行梯度下降
def grad_descent(x_input, y_input, w_input, b_input, num_iters, learning_rate, cost_function, grad_function):
    history_cost = []
    history_param = []
    w = copy.deepcopy(w_input)
    b = copy.deepcopy(b_input)
    for i in range(num_iters):
        grad_list = grad_function(x_input, y_input, w, b)
        w -= learning_rate * grad_list[0:len(x_input[0])]
        b -= learning_rate * grad_list[-1]
        if i < 100000:
            history_cost.append(cost_function(x_input, y_input, w, b))
            history_param.append([w, b])
        if i % math.ceil(num_iters / 10) == 0 and i != 0:
            print(f"迭代次数:{i:4},当前损失值:{history_cost[-1]:0.4e},当前w:{w},当前b:{b}")
    return w, b, history_cost, history_param


# 进行z_score均一化
def z_score(x, y):
    x_mean = np.mean(x, axis=0)
    x_dev = np.std(x, axis=0)
    y_mean = np.mean(y, axis=0)
    y_dev = np.std(y, axis=0)
    mean = np.append(x_mean, y_mean)
    dev = np.append(x_dev, y_dev)
    n = x[0].shape[0]
    m = y.shape[0]
    # 对列进行均一化
    for i in range(n):
        if -3 <= mean[i] <= 3:
            continue
        for j in range(m):
            x[j][i] = (x[j][i] - mean[i]) / dev[i]
    if mean[-1] >= 3 or mean[-1] <= -3:
        for i in range(m):
            y[i] = (y[i] - mean[-1]) / dev[-1]
    return mean, dev


# 准备数据
def prepare_date(url):
    _x = []
    x_list = []
    y_list = []
    with open('./' + url, 'r') as file:
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


# 进行训练
x_input, y_input = prepare_date('houses.txt')
x_input = np.array(x_input)
y_input = np.array(y_input)
w_input = np.zeros(x_input[0].shape[0])
mean, dev = z_score(x_input, y_input)
b_input = 0.0
learning_rate = 1.0e-3
num_iters = 10000
w_final, b_final, cost_list, param_list = grad_descent(x_input, y_input, w_input, b_input, num_iters, learning_rate,
                                                       compute_cost, compute_grad)
print(f"(w,b) found by gradient descent: ({w_final},{b_final})")

# 创建损失值变化图
iterations_list = np.arange(1, num_iters + 1)
plt.plot(iterations_list, cost_list, marker='o', linestyle='-', color='b', label='Loss')
# 添加标签和标题
plt.xlabel('Iteration times')
plt.ylabel('Loss value')
plt.title('Loss')

plt.show()

# 进行预测
while 1:
    user_input = input(f"请输入整数列表（用逗号分隔元素,{x_input[0].shape[0]}个参数）: ")
    input_list = user_input.split(',')  # 以逗号分隔字符串并创建一个字符串列表
    x_input_list = np.array([float(x) for x in input_list])  # 将字符串列表转换为整数列表
    for i in range(x_input[0].shape[0]):
        if -3 <= mean[i] <= 3:
            continue
        x_input_list[i] = (x_input_list[i] - mean[i]) / dev[i]
    ans = np.dot(x_input_list, w_final) + b_final
    if mean[-1] >= 3 or mean[-1] <= -3:
        ans = ans * dev[-1] + mean[-1]
    print(f"\n预测结果为{ans}")
