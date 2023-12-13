import os

import numpy as np
import pandas as pd
import tensorflow as tf
from keras import optimizers

physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True)
os.environ['CUDA_VISIBLE_DEVICES'] = '/gpu:0'


def cofiCostfunc(X, W, b, Y, R, lamda_):
    """
    :param X: 物的特征矩阵
    :param W: 用户偏好矩阵
    :param b: 用户偏好修正矩阵
    :param Y: 训练用数据集
    :param R: 记录矩阵
    :param lamda_: 正则化参数
    :return: 基于协同过滤算法计算当前损失值
    """

    # 损失值的计算基于误差的平方
    j = tf.square((tf.linalg.matmul(X, tf.transpose(W)) + b - Y)) * R
    # 对每个误差项的平方求和后除以2再加上对每个参数的L2正则化项
    cost = tf.reduce_sum(j) / 2 + (lamda_ / 2) * (tf.reduce_sum(tf.square(X)) + tf.reduce_sum(tf.square(W)))
    return cost


def normalizeRatings(Y, R):
    """
    针对行为矩阵中为1的项，对每个物求得分的均值，并将每个物的得分减去其均值。目标使物的得分的均值为0
    """
    Ymean = (np.sum(Y * R, axis=1) / (np.sum(R, axis=1) + 1e-12)).reshape(-1, 1)
    Ynorm = Y - np.multiply(Ymean, R)
    return Ynorm, Ymean


# 准备数据
y_train = np.array(pd.read_csv("data_for_collaborate_filter/small_movies_Y.csv"))
r_train = np.array(pd.read_csv("data_for_collaborate_filter/small_movies_R.csv"))

y_norm, y_mean = normalizeRatings(y_train, r_train)
nm, nu = y_train.shape
num_features = 100  # 假设特征为100个
# 初始化参数矩阵
X = tf.Variable(tf.random.normal((nm, num_features), dtype=tf.float64), name="X")
W = tf.Variable(tf.random.normal((nu, num_features), dtype=tf.float64), name="W")
b = tf.Variable(tf.random.normal((1, nu), dtype=tf.float64), name="b")
# 训练
iterations = 800
lamda_ = 0.01
optimizer = optimizers.Adam(learning_rate=1e-1)
with tf.device('/GPU:0'):
    for iter in range(iterations):
        with tf.GradientTape() as tape:
            # 计算损失值
            cost = cofiCostfunc(X, W, b, y_norm, r_train, lamda_)
        # 计算梯度
        grads = tape.gradient(cost, [W, X, b])
        # 使用优化器更新参数
        optimizer.apply_gradients(zip(grads, [W, X, b]))
        # Log periodically.
        if iter % 20 == 0:
            print(f"Training loss at iteration {iter}: {cost:0.1f}")
