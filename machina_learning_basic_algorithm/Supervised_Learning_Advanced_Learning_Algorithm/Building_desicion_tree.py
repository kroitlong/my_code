import numpy as np


def compute_entropy(y):
    """
    计算当前子集y的信息熵
    Args:
       y (ndarray): 待计算信息熵的结点子集y
    Returns:
        entropy (float): 该节点的信息熵

    """
    entropy = 0.

    count = np.count_nonzero(y == 1)
    # 子集非空且子集非纯
    if len(y) != 0 and count != len(y) and count != 0:
        p_1 = count / len(y)
        entropy = -(p_1 * np.log2(p_1) + (1 - p_1) * (np.log2(1 - p_1)))

    return entropy


def split_dataset(X, node_indices, feature):
    """
    根据给出的特征号分割当前结点的数据到左子节点和右子节点
    Args:
        X (ndarray):             数据矩阵(n_samples, n_features)
        node_indices (ndarray):  当前待分割结点所含的数据集合（以在数据矩阵X中的下标表示）
        feature (int):           特征的index
    Returns:
        left_indices (ndarray):  one-hot对应特征为1
        right_indices (ndarray): one-hot对应特征为0
    """

    left_indices = []
    right_indices = []

    for i in node_indices:
        if X[i, feature] == 1:
            left_indices.append(i)
        else:
            right_indices.append(i)

    return left_indices, right_indices


def compute_information_gain(X, y, node_indices, feature):
    """
    计算根据给定特征分割结点后的信息增益
    Args:
        X (ndarray):            数据矩阵(n_samples, n_features)
        y (array like):         数据矩阵中每个样本对应的标签
        node_indices (ndarray): 当前结点的标签集合
        feature(int):           特征的index
    Returns:
        cost (float):        计算的信息增益

    """
    # 分割
    left_indices, right_indices = split_dataset(X, node_indices, feature)

    # 根据node_indices对原数据集进行切片
    X_node, y_node = X[node_indices], y[node_indices]
    X_left, y_left = X[left_indices], y[left_indices]
    X_right, y_right = X[right_indices], y[right_indices]

    # 计算左右子树的权重
    w_left = len(left_indices) / len(node_indices)
    w_right = len(right_indices) / len(node_indices)
    # 计算左右子树和当前结点的信息熵
    entropy_left = compute_entropy(y_left)
    entropy_right = compute_entropy(y_right)
    entropy_root = compute_entropy(y_node)
    # 根据权重和信息熵计算信息增益
    information_gain = entropy_root - (w_left * entropy_left +
                                       w_right * entropy_right)

    return information_gain


def get_best_split(X, y, node_indices):
    """
    获得按特征分割后信息增益最多的特征的index

    Args:
        X (ndarray):            数据矩阵(n_samples, n_features)
        y (array like):         数据矩阵中每个样本对应的标签
        node_indices (ndarray): 当前结点对应的样本的标签集合

    Returns:
        best_feature (int):     信息增量最多的特征的index
    """

    num_features = X.shape[1]

    best_feature = -1

    information_gain = 0
    for i in range(num_features):
        cur_information_gain = compute_information_gain(X, y, node_indices, i)
        if cur_information_gain > information_gain:
            information_gain = cur_information_gain
            best_feature = i

    return best_feature


tree = []


def build_tree_recursive(X, y, node_indices, branch_name, max_depth, current_depth):
    """
    递归的构建决策树

    Args:
        X (ndarray):            数据矩阵(n_samples, n_features)
        y (array like):         样本对应的标签集合
        node_indices (ndarray): 当前结点对应的样本标签的集合
        branch_name (string):   当前结点对应的名称 ['Root', 'Left', 'Right']
        max_depth (int):        构建的决策树的深度
        current_depth (int):    当前的深度，决定合适停止递归

    """

    # 判断是否停止递归
    if current_depth == max_depth:
        formatting = " " * current_depth + "-" * current_depth
        print(formatting, "%s leaf node with indices" % branch_name, node_indices)
        return

    # 获得当前结点的导致信息增益最多的分割特征
    best_feature = get_best_split(X, y, node_indices)
    tree.append((current_depth, branch_name, best_feature, node_indices))

    formatting = "-" * current_depth
    print("%s Depth %d, %s: Split on feature: %d" % (formatting, current_depth, branch_name, best_feature))

    # 根据之前选择的特征对当前节点进行分割
    left_indices, right_indices = split_dataset(X, node_indices, best_feature)

    # 递归的处理左节点和右节点
    build_tree_recursive(X, y, left_indices, "Left", max_depth, current_depth + 1)
    build_tree_recursive(X, y, right_indices, "Right", max_depth, current_depth + 1)


# 测试
X_train = np.array(
    [[1, 1, 1], [1, 0, 1], [1, 0, 0], [1, 0, 0], [1, 1, 1], [0, 1, 1], [0, 0, 0], [1, 0, 1], [0, 1, 0], [1, 0, 0]])
y_train = np.array([1, 1, 0, 0, 1, 0, 0, 1, 1, 0])
root_indices = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
build_tree_recursive(X_train, y_train, root_indices, "Root", max_depth=2, current_depth=0)
print(tree)
