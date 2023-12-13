import keras
import numpy as np
import tensorflow as tf
from keras.layers import Normalization
from sklearn.metrics import accuracy_score

# 手写数字识别模型的测试
# 导入数据
with open(r'hand_write_digit\train-images.idx3-ubyte', 'rb') as f:
    x_train = np.frombuffer(f.read(), np.uint8, offset=16).reshape(-1, 784)
with open(r'hand_write_digit\train-labels.idx1-ubyte', 'rb') as f:
    y_train = np.frombuffer(f.read(), np.uint8, offset=8)

with open(r'hand_write_digit\t10k-images.idx3-ubyte', 'rb') as f:
    x_test = np.frombuffer(f.read(), np.uint8, offset=16).reshape(-1, 784)
with open(r'hand_write_digit\t10k-labels.idx1-ubyte', 'rb') as f:
    y_test = np.frombuffer(f.read(), np.uint8, offset=8)

# 获取预处理参数
norm_l = keras.layers.Normalization(axis=-1)
norm_l.adapt(x_train)
x_train = norm_l(x_train)
x_test = norm_l(x_test)

model = keras.models.load_model('models/model2.h5')
test_y_get = tf.nn.softmax(model.predict(x_test, batch_size=64))
test_y_result = []
for i in range(test_y_get.shape[0]):
    test_y_result.append(np.argmax(test_y_get[i]))
# test_accuracy = 0
# for i in range(test_y_get.shape[0]):
#     if np.argmax(test_y_get[i]) == y_test[i]:
#         test_accuracy += 1
#
train_y_get = tf.nn.softmax(model.predict(x_train, batch_size=64))
train_y_result = []
for i in range(train_y_get.shape[0]):
    train_y_result.append(np.argmax(train_y_get[i]))
# train_accuracy = 0
# for i in range(train_y_get.shape[0]):
#     if np.argmax(train_y_get[i]) == y_train[i]:
#         train_accuracy += 1
#
# print("test_accuracy:", test_accuracy / y_test.shape[0])
# print("train_accuracy:", train_accuracy / y_train.shape[0])

print(f"test_accuracy:{accuracy_score(y_test, test_y_result)}")
print(f"train_accuracy:{accuracy_score(y_train, train_y_result)}")
