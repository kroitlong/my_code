import numpy as np


def estimate_gaussian(X):
    """
   计算所有特征的均值和方差

    Args:
        X (ndarray): (m, n) 数据矩阵

    Returns:
        mu (ndarray): (n), 所有特征的均值的list
        var (ndarray): (n), 所有特征的方差的list
    """

    m, n = X.shape

    mu = np.zeros(n)
    var = np.zeros(n)
    for i in range(m):
        mu += X[i]
    mu /= m
    for i in range(m):
        var += (X[i] - mu) ** 2
    var /= m

    return mu, var


def select_threshold(y_val, p_val):
    """
    根据测试集中预测值和真实值找到合适的阈值作为异常检测的阈值。这里基于F1-Score来进行评价

    Args:
        y_val (ndarray): 测试集上的Ground truth
        p_val (ndarray): 测试集上的结果

    Returns:
        epsilon (float): 选择的阈值
        F1 (float):      该阈值下的F1-Score
    """

    best_epsilon = 0
    best_F1 = 0
    F1 = 0

    # 每次阈值移动的步长
    step_size = (max(p_val) - min(p_val)) / 1000
    for epsilon in np.arange(min(p_val), max(p_val), step_size):
        tp, fp, fn = [0, 0, 0]
        for i in range(len(p_val)):
            if p_val[i] >= epsilon and y_val[i] == 1:
                # 预测为0实际为1表示false negative
                fn += 1
            elif p_val[i] < epsilon and y_val[i] == 1:
                # 预测为1实际为1为true positive
                tp += 1
            elif p_val[i] < epsilon and y_val[i] == 0:
                # 预测为1实际为0表示false positive
                fp += 1
        if tp != 0:
            precise = tp / (tp + fp)
            recall = tp / (tp + fn)
            F1 = 2 * precise * recall / (precise + recall)

        else:
            F1 = 0

        if F1 > best_F1:
            best_F1 = F1
            best_epsilon = epsilon

    return best_epsilon, best_F1


def multivariate_gaussian(x, mu, var):
    """

    :param x: 待检测的数据集
    :param mu: 所有特征的均值list
    :param var: 所有特征的方差list
    :return: 每个样本的高斯概率
    """
    x_probability = []
    for i in range(x.shape[0]):
        x_i_probability = 1
        for j in range(x.shape[1]):
            x_i_probability *= 1 / ((2 * np.pi * var[j]) ** 0.5) * np.exp(-1 * (x[i][j] - mu[j]) ** 2 / (2 * var[j]))
        x_probability.append(x_i_probability)
    return x_probability


x_train = np.load('data_for_kmeans_and_anomaly_detection/X_part2.npy')
x_val = np.load('data_for_kmeans_and_anomaly_detection/X_val_part2.npy')
y_val = np.load('data_for_kmeans_and_anomaly_detection/y_val_part2.npy')
print("训练集-验证集-验证集的标签的shape:", x_train.shape, x_val.shape, y_val.shape)

# 计算数据集特征的均值list和方差list
X_mu, X_var = estimate_gaussian(x_train)

# 计算训练集的高斯概率集合
p_x_train = multivariate_gaussian(x_train, X_mu, X_var)

# 计算验证集的高斯概率集合
p_x_var = multivariate_gaussian(x_val, X_mu, X_var)

epsilon, F1_score = select_threshold(y_val, p_x_var)
print('Best epsilon found using cross-validation: %e' % epsilon)
print('Best F1 on Cross Validation Set:  %f' % F1_score)
print('# Anomalies found: %d' % sum(p_x_train < epsilon))
