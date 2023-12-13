import numpy as np
import tensorflow as tf
from keras import Model
from numpy import genfromtxt
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from tensorflow._api.v2.compat.v1 import keras

# ------------------------------------------------------------------------------------------------------------ #
# 加载数据
item_train = genfromtxt(r"data_for_content_based_filter/content_item_train.csv", delimiter=',')
user_train = genfromtxt(r"data_for_content_based_filter/content_user_train.csv", delimiter=',')
item_features = genfromtxt(r'data_for_content_based_filter/content_item_train_header.txt', delimiter=',')
user_features = genfromtxt(r"data_for_content_based_filter/content_user_train_header.txt", delimiter=',')
item_vecs = genfromtxt(r"data_for_content_based_filter/content_item_vecs.csv", delimiter=',')
label_y = genfromtxt(r"data_for_content_based_filter/content_y_train.csv", delimiter=',')
num_user_features = len(user_features) - 3  # 前三个特征不参与训练
num_item_features = len(item_features) - 1  # 第一个特征不参与训练

# ------------------------------------------------------------------------------------------------------------ #
# 对数据进行预处理
item_train_save = item_train
user_train_save = user_train
y_save = label_y

# 创建标准化器进行标准化
scalerItem = StandardScaler()
scalerItem.fit(item_train)
item_train = scalerItem.transform(item_train)
# 创建标准化器进行标准化
scalerUser = StandardScaler()
scalerUser.fit(user_train)
user_train = scalerUser.transform(user_train)
# 创建Min-Max归一化器进行归一化
scaler_y = MinMaxScaler()
scaler_y.fit(label_y.reshape(-1, 1))
ynorm_train = scaler_y.transform(label_y.reshape(-1, 1))

# 对数据进行划分，划分为训练集和测试集
user_train, user_test, item_train, item_test, y_train, y_test = train_test_split(user_train, item_train, ynorm_train,
                                                                                 train_size=0.8, shuffle=True,
                                                                                 random_state=1)

# ------------------------------------------------------------------------------------------------------------ #
# 搭建训练网络
num_outputs = 32
tf.random.set_seed(1)
user_NN = tf.keras.models.Sequential([
    tf.keras.layers.Dense(256, activation="relu"),
    tf.keras.layers.Dense(128, activation="relu"),
    tf.keras.layers.Dense(num_outputs, activation="linear"),
])

item_NN = tf.keras.models.Sequential([
    tf.keras.layers.Dense(256, activation="relu"),
    tf.keras.layers.Dense(128, activation="relu"),
    tf.keras.layers.Dense(num_outputs, activation="linear"),
])
# 创建usr输入层
input_user = tf.keras.layers.Input(shape=num_user_features)
vu = user_NN(input_user)
# 输入传出网络后进行l2归一化
vu = tf.linalg.l2_normalize(vu, axis=1)

# 创建item输入层
input_item = tf.keras.layers.Input(shape=num_item_features)
vm = item_NN(input_item)
# 输入传出网络后进行l2归一化
vm = tf.linalg.l2_normalize(vm, axis=1)

# 构建模型输出层，对两个网络的输出取点积
output = tf.keras.layers.Dot(axes=1)([vu, vm])

# 构建摩西
model = Model([input_user, input_item], output)

# 打印模型信息
model.summary()

# ------------------------------------------------------------------------------------------------------------ #
# 训练模型
tf.random.set_seed(1)
cost_fn = tf.keras.losses.MeanSquaredError()  # 定义损失函数
opt = keras.optimizers.Adam(learning_rate=0.01)  # 定义优化器
model.compile(optimizer=opt, loss=cost_fn)  # 模型编译
# 选择的是后面的特征，所以对列进行了一个切片
model.fit([user_train[:, 3:], item_train[:, 1:]],
          y_train,
          epochs=30,
          batch_size=64)
