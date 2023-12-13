import numpy as np


def find_closest_centroids(X, centroids):
    """
    计算每个样本点当前最近的簇的编号

    Args:
        X (ndarray): (m, n) 样本集合
        centroids (ndarray): k个簇心

    Returns:
        idx (array_like): 一个列表，表示每个样本对应的最近的簇的中心index

    """

    K = centroids.shape[0]

    idx = np.zeros(X.shape[0], dtype=int)

    min_dis = np.finfo(np.float64).max
    for i in range(X.shape[0]):
        for j in range(K):
            if np.linalg.norm(X[i] - centroids[j]) ** 2 < min_dis:
                min_dis = np.linalg.norm(X[i] - centroids[j]) ** 2
                idx[i] = j
        min_dis = np.finfo(np.float64).max

    return idx


def compute_centroids(X, idx, K):
    """
    根据为每个簇心划分的数据的集合，更新每个簇心的坐标

    Args:
        X (ndarray):   (m, n) 所有的数据点
        idx (ndarray): (m), 每个样本点对应的簇心的index
        K (int):       簇心的数量

    Returns:
        centroids (ndarray): 更新后新的簇心的集合
    """

    m, n = X.shape

    centroids = np.zeros((K, n))

    length = np.zeros(K)
    for i in range(m):
        centroids[idx[i]] += X[i]
        length[idx[i]] += 1
    for i in range(K):
        centroids[i] = centroids[i] / length[i]

    return centroids


def run_kMeans(X, initial_centroids, max_iters=10):
    """
   执行K-Means算法
    """

    # 初始化簇心
    m, n = X.shape
    K = initial_centroids.shape[0]
    centroids = initial_centroids
    idx = np.zeros(m)

    for i in range(max_iters):
        # 输出执行过程中簇心的更新信息
        print("K-Means iteration %d/%d" % (i, max_iters - 1))

        # 对每个样本点，将其赋给离它最近的簇心
        idx = find_closest_centroids(X, centroids)

        # 更新簇心
        centroids = compute_centroids(X, idx, K)
    return centroids, idx


def kMeans_init_centroids(X, K):
    """
    基于样本集合初始化簇心

    Args:
        X (ndarray): 数据点的集合
        K (int):     簇的数量

    Returns:
        centroids (ndarray): 初始化后的簇心的集合
    """

    # 随机打乱原数据集
    rand_idx = np.random.permutation(X.shape[0])

    # 取前K个作为簇心
    centroids = X[rand_idx[:K]]

    return centroids


# 算法测试
Data = np.load('data_for_kmeans_and_anomaly_detection/ex7_X.npy')
print(Data.shape)
initial_centroids = kMeans_init_centroids(Data, 3)
centroids, idx = run_kMeans(Data, initial_centroids, 10)
print(centroids)
