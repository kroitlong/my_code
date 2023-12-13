import os
import numpy as np
import tensorflow as tf
from tensorflow import keras
from keras.layers import Dense, Conv2D, convolutional, Normalization
from keras.models import Sequential

physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True)
os.environ['CUDA_VISIBLE_DEVICES'] = '/gpu:0'

# 使用导入数据
with open(r'hand_write_digit\train-images.idx3-ubyte', 'rb') as f:
    x_train = np.frombuffer(f.read(), np.uint8, offset=16).reshape(-1, 784)
with open(r'hand_write_digit\train-labels.idx1-ubyte', 'rb') as f:
    y_train = np.frombuffer(f.read(), np.uint8, offset=8)
# 数据预处理
norm_l = Normalization(axis=-1)
norm_l.adapt(x_train)
x_train = norm_l(x_train)

# 搭建网络结构
model = Sequential([
    Dense(units=300, activation='relu', name='l1', kernel_regularizer=keras.regularizers.L2(0.01)),
    Dense(units=100, activation='relu', name='l2', kernel_regularizer=keras.regularizers.L2(0.01)),
    Dense(units=75, activation='relu', name='l3', kernel_regularizer=keras.regularizers.L2(0.01)),
    Dense(units=10, activation='linear', name='output'),
])

# 编译模型，采用二元交叉熵损失函数
model.compile(loss=keras.losses.SparseCategoricalCrossentropy(from_logits=True),
              optimizer=keras.optimizers.Adam(1.0e-3), )

try:
    with tf.device('/GPU:0'):
        # 训练模型，迭代次数epochs=15
        model.fit(x_train, y_train, epochs=20, batch_size=64)
except RuntimeError as e:
    print(e)
# 保存模型,路径中不能有中文名
model.save('models/model2.h5')

# 显示模型的基本信息
model.summary()
