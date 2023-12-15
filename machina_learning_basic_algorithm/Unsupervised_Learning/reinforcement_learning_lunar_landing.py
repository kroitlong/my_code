import logging
import os
import time
from collections import deque, namedtuple

import gym
import numpy as np
import tensorflow as tf
from keras import Sequential
from keras.layers import Dense, Input
from keras.losses import MSE
from keras.optimizers import Adam
from tensorflow.python.keras.models import load_model

import utils

# 使用GPU加速计算
physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True)
os.environ['CUDA_VISIBLE_DEVICES'] = '/gpu:0'

# 设置随机数种子，以再现
tf.random.set_seed(utils.SEED)

MEMORY_SIZE = 100_000  # 设置MEMORY_BUFFER的大小
GAMMA = 0.995  # 折扣因子
ALPHA = 1e-3  # 学习率，用于Adam学习率的是指
NUM_STEPS_FOR_UPDATE = 4  # 更新参数的步数，每4步更新了一次参数

env = gym.make('LunarLander-v2', render_mode='rgb_array')  # 创建环境，设置渲染模式

state_size = env.observation_space.shape
num_actions = env.action_space.n

print('State Shape:', state_size)
print('Number of actions:', num_actions)

# 重置环境
initial_state = env.reset()

# 创建 Q-Network
q_network = Sequential([
    Input(shape=state_size),
    Dense(64, activation='relu'),
    Dense(64, activation='relu'),
    Dense(num_actions, activation='linear'),
])

# 创建用于计算target的网络 Q^-Network，这两个网络的结构是一样的，都是用来预测Q(s,a)
target_q_network = Sequential([
    Input(shape=state_size),
    Dense(64, activation='relu'),
    Dense(64, activation='relu'),
    Dense(num_actions, activation='linear'),
])

optimizer = Adam(learning_rate=ALPHA)

# 创建记录用的tuple
experience = namedtuple("Experience", field_names=["state", "action", "reward", "next_state", "done"])


def compute_loss(experiences, gamma, q_network, target_q_network):
    """
    计算损失值

    Args:
      experiences: (tuple) ["state", "action", "reward", "next_state", "done"] namedtuples
      gamma: (float) 折扣因子
      q_network: (tf.keras.Sequential) 训练的网络
      target_q_network: (tf.keras.Sequential) 用于计算target的网络

    Returns:
      loss: (TensorFlow Tensor(shape=(0,), dtype=int32)) 均方误差sum[0-m](yi-q_network(state))**2
    """

    # 将minibatch解包
    states, actions, rewards, next_states, done_vals = experiences

    # 计算 max Q^(s,a)
    max_qsa = tf.reduce_max(target_q_network(next_states), axis=-1)

    # 如果到了终态设置y = R, 否则 y = R + γ max Q^(s,a).即使用贝尔曼方程进行计算
    y_targets = rewards + gamma * (1 - done_vals) * max_qsa

    # 获取预测值
    q_values_get = q_network(states)
    # 由于神经网络输出了四个值，对应了四个动作产生的Q(s,a),这一步是在通过actions列表获取q_values中的对应动作的值
    q_values_get = tf.gather_nd(
        q_values_get,
        tf.stack([tf.range(q_values_get.shape[0]),
                  tf.cast(actions, tf.int32)],
                 axis=1))
    # 调用keras.losses中的MSE模块计算均方误差
    loss = MSE(y_targets, q_values_get)
    return loss


# 通过之前学过的with tf.GradientTape() as Tape先计算损失值，再根据Tape.gradient(loss,variate_list)计算variate_list
# 的梯度，并存储在gradients中，再使用optimalize.alply_gradients(zip(gradients,variate_list))来对变量列表更新梯度
@tf.function
def agent_learn(experiences, gamma):
    """
    更新网络的权重

    Args:
      experiences: (tuple) ["state", "action", "reward", "next_state", "done"] namedtuples
      gamma: (float) 折扣因子

    """

    # 先使用上面的函数计算损失值
    with tf.GradientTape() as tape:
        loss = compute_loss(experiences, gamma, q_network, target_q_network)

    # 根据损失值计算网络中所以变量的梯度(使用反向传播)
    gradients = tape.gradient(loss, q_network.trainable_variables)

    # 利用优化器更新参数
    optimizer.apply_gradients(zip(gradients, q_network.trainable_variables))

    # 同时更新计算target的网络中的参数
    utils.update_target_network(q_network, target_q_network)


# 使用GPU加速计算
with tf.device('/GPU:0'):
    start = time.time()

    num_episodes = 2000
    max_num_timesteps = 1000

    total_point_history = []

    num_p_av = 100  # 每100次训练计算一次平均分
    epsilon = 1.0  # 用于初始话ε-greedy策略的值, 1表示初始时随机选择动作

    # 创建大小为MEMORY_SIZE的MEMORY_BUFFER
    memory_buffer = deque(maxlen=MEMORY_SIZE)

    # 初始化计算target的网络的权重等于训练网络的权重
    target_q_network.set_weights(q_network.get_weights())
    # 训练过的话直接加载模型即可
    if os.path.exists("./models/lunar_lander_model.h5"):
        q_network = load_model("./models/lunar_lander_model.h5")
        train = False
    else:
        train = True

    # 开始训练
    if train:
        for i in range(num_episodes):

            # 每轮训练要重置环境
            state, _ = env.reset()
            total_points = 0

            for t in range(max_num_timesteps):

                # 基于ε-greedy策略从当前状态选择一个动作A
                state_qn = np.expand_dims(
                    state,
                    axis=0)  # 处理训练数据到需要的形状
                q_values = q_network(state_qn)
                # 获取动作A
                action = utils.get_action(q_values, epsilon)

                # 根据选择的动作A，执行并得到关于执行该动作后的信息
                next_state, reward, done, _, _ = env.step(action)

                # 将执行后的信息存储在MEMORY_BUFFER中
                memory_buffer.append(
                    experience(state, action, reward, next_state, done))

                # 检查是否满足更新参数的条件，前面提到过每NUM_STEPS_FOR_UPDATE次更新一次参数，这是为了防止训练出现严重的震荡或者不稳定
                update = utils.check_update_conditions(t, NUM_STEPS_FOR_UPDATE,
                                                       memory_buffer)

                if update:
                    # 从MEMOEY_BUFFER中随机的选择一个mini_batch(64个)出来进行参数的更新，这样是为了加快训练速度
                    experiences = utils.get_experiences(memory_buffer)
                    # 调用前面的函数进行参数的更新
                    agent_learn(experiences, GAMMA)

                # 更新当前状态为执行动作后的下一状态
                state = next_state.copy()

                # 增加此次执行后获得的分数
                total_points += reward

                if done:
                    break

            total_point_history.append(total_points)
            av_latest_points = np.mean(total_point_history[-num_p_av:])

            # 更新ε-greedy策略的值，使得其对动作的选择具有一定的偏向性
            epsilon = utils.get_new_eps(epsilon)

            # 打印训练信息
            print(
                f"\rEpisode {i + 1} | Total point average of the last {num_p_av} episodes: {av_latest_points:.2f}",
                end="")

            if (i + 1) % num_p_av == 0:
                print(
                    f"\rEpisode {i + 1} | Total point average of the last {num_p_av} episodes: {av_latest_points:.2f}"
                )

            # 我们认为当累计分数达到200后则训练完成
            if av_latest_points >= 200.0:
                print(f"\n\nEnvironment solved in {i + 1} episodes!")
                q_network.save('./models/lunar_lander_model.h5')
                break

        tot_time = time.time() - start

        print(f"\nTotal Runtime: {tot_time:.2f} s ({(tot_time / 60):.2f} min)")

# 收集运行imageio中的报错信息(忽略)
logging.getLogger().setLevel(logging.ERROR)

# 创建一个video用与可视化展示训练成功
filename = "./videos/lunar_lander.mp4"
utils.create_video(filename, env, q_network)
utils.embed_mp4(filename)
