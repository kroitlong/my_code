import numpy as np
import tensorflow as tf
from tensorflow import keras
from keras.layers import Dense
from keras.layers import Normalization
from keras import Sequential
from my_package.tool import load_data
from my_package.tool import expand_expression

physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True)

x_train, y_train = load_data('data/training_data.txt')

# 这里拟合一个二次曲线,再处理一下x_train，调用工具库中的函数进行升维处理
for m in range(len(x_train)):
    x_train[m] = expand_expression(x_train[m], 2)
x_train = np.array(x_train)
y_train = np.array(y_train)

# 对数据进行归一化
norm_l = Normalization(axis=-1)
norm_l.adapt(x_train)
x_train = norm_l(x_train)

# 构建模型
tf.random.set_seed(1234)
model = Sequential([
    Dense(units=25, activation='relu', name='l1'),
    Dense(units=15, activation='relu', name='l2'),
    Dense(units=1, activation='linear', name='output'),
])

# 编译模型，采用二元交叉熵损失函数
model.compile(loss=keras.losses.BinaryCrossentropy(from_logits=1),
              optimizer=keras.optimizers.Adam(0.01), )

with tf.device('/GPU:0'):
    # 训练模型，迭代次数epochs=100
    model.fit(x_train, y_train, epochs=100)

# 保存模型,路径中不能有中文名
model.save('models/model1.h5')

# 显示模型的基本信息
model.summary()
