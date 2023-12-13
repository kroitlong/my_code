import numpy as np
import matplotlib.pyplot as plt
import copy
import math


# 计算损失函数的值
def compute_cost(x, y, w, b):
    cost = 0
    m = x.shape[0]
    for i in range(m):
        temp = (w * x[i] + b - y[i]) ** 2
        cost += temp
    cost /= 2 * m
    return cost


# 计算梯度
def compute_grad(x, y, w, b):
    grad_w = 0
    grad_b = 0
    m = x.shape[0]
    for i in range(m):
        temp = w * x[i] + b - y[i]
        temp_w = temp * x[i]
        grad_w += temp_w
        grad_b += temp
    grad_w /= m
    grad_b /= m
    return grad_w, grad_b


# 进行梯度下降
def grad_descent(x, y, w_in, b_in, num_iter, learning_rate, compute_cost_func, compute_grad_func):
    w = copy.deepcopy(w_in)
    b = copy.deepcopy(b_in)
    history_cost = [compute_cost_func(x, y, w, b)]
    history_param = [[w, b]]

    for i in range(num_iter):
        grad_w, grad_b = compute_grad_func(x, y, w, b)
        w -= learning_rate * grad_w
        b -= learning_rate * grad_b
        if i < 100000:
            history_cost.append(compute_cost_func(x, y, w, b))
            history_param.append([w, b])
        if i % math.ceil(num_iter / 10) == 0:
            print(f"迭代次数:{i:4},当前损失值:{history_cost[-1]:0.4e},当前w:{w:0.3e},当前b:{b:0.3e}")
    return w, b, history_cost, history_param


# 主函数
# __name__ = '__main__'
# 创建用于存储不同数据的列表
x_list = []
y_list = []
with open('./data.txt', 'r') as file:
    for line in file:
        # 使用逗号分割数据
        parts = line.split(',')
        # 将分割后的数据添加到不同的列表
        x_list.append(float(parts[0].strip()) / 1000)
        y_list.append(float(parts[2].strip()) / 1000)

# 用于准备数据
x_train = np.array(x_list)
y_train = np.array(y_list)
w_ini = 0
b_ini = 0
learning_rate = 1.0e-3
num_iter = 1000
w_final, b_final, cost_list, param_list = grad_descent(x_train, y_train, w_ini, b_ini, num_iter, learning_rate,
                                                       compute_cost, compute_grad)
print(f"(w,b) found by gradient descent: ({w_final:8.4f},{b_final:8.4f})")
model_x = np.linspace(0, 5, 2)
model_y = w_final * model_x + b_final
iterations_list = np.arange(1, num_iter + 2)

# 创建散点图
plt.figure(1)
plt.scatter(x_list, y_list, label='数据点', color='blue', marker='o')
plt.plot(model_x, model_y, label='直线', color='red', linestyle='--')
# 添加标签和标题
plt.xlabel('Size(1000/square feets)')
plt.ylabel('Price(1000k$)')
plt.title('Data set')
plt.legend('Date point')

# 创建损失值变化图
plt.figure(2)
plt.plot(iterations_list, cost_list, marker='o', linestyle='-', color='b', label='Loss')
# 添加标签和标题
plt.xlabel('Iteration times')
plt.ylabel('Loss value')
plt.title('Loss')

plt.show()
