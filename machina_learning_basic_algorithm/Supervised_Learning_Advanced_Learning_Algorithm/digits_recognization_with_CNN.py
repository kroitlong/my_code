import os
import numpy as np
import tensorflow as tf
from keras import Sequential
from keras.layers import Dense, Conv2D, BatchNormalization, MaxPool2D, Activation, Flatten, Normalization
from tensorflow import keras

physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True)
os.environ['CUDA_VISIBLE_DEVICES'] = '/gpu:0'

# 使用导入数据
with open(r'hand_write_digit\train-images.idx3-ubyte', 'rb') as f:
    x_train = np.frombuffer(f.read(), np.uint8, offset=16).reshape(-1, 28, 28)
with open(r'hand_write_digit\train-labels.idx1-ubyte', 'rb') as f:
    y_train = np.frombuffer(f.read(), np.uint8, offset=8)
# 数据预处理
norm_l = Normalization(axis=-1)
norm_l.adapt(x_train)
x_train = norm_l(x_train)

with tf.device('/GPU:0'):
    # 搭建CNN模型
    model = Sequential([
        Conv2D(filters=16, kernel_size=3, strides=1, activation='linear', name='cov1',
               kernel_regularizer=keras.regularizers.L2(0.01), input_shape=(28, 28, 1)),
        BatchNormalization(),
        Activation('relu'),
        Conv2D(filters=32, kernel_size=3, strides=1, activation='linear', name='cov2',
               kernel_regularizer=keras.regularizers.L2(0.01), input_shape=(26, 26, 16)),
        BatchNormalization(),
        Activation('relu'),
        MaxPool2D((2, 2)),
        Conv2D(filters=64, kernel_size=3, strides=1, padding='same', activation='linear', name='cov3',
               kernel_regularizer=keras.regularizers.L2(0.01), input_shape=(12, 12, 32)),
        BatchNormalization(),
        Activation('relu'),
        MaxPool2D((2, 2)),
        Conv2D(filters=32, kernel_size=3, strides=1, padding='same', activation='relu', name='cov4',
               kernel_regularizer=keras.regularizers.L2(0.01), input_shape=(6, 6, 32)),
        Flatten(),
        Dense(units=75, activation='relu', name='l1', kernel_regularizer=keras.regularizers.L2(0.01)),
        Dense(units=25, activation='relu', name='l2', kernel_regularizer=keras.regularizers.L2(0.01)),
        Dense(units=10, activation='linear', name='output', ),
    ])
    model.compile(loss=keras.losses.SparseCategoricalCrossentropy(from_logits=True),
                  optimizer=keras.optimizers.Adam(1.0e-3), )
    model.fit(x_train, y_train, epochs=30, batch_size=100)

# 保存模型,路径中不能有中文名
model.save('models/model3.h5')

# 显示模型的基本信息
model.summary()
