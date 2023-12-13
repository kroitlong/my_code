import matplotlib.pyplot as plt
import numpy as np


def relu(x):
    return np.maximum(0, x)


def hard_swish(x):
    return x * np.maximum(0, np.minimum(6, x + 3)) / 6


def swish(x):
    return x * 1 / (1 + np.exp(-x))


x = np.linspace(-4, 4, 100)
y_relu = relu(x)
y_hard_swish = hard_swish(x)
y_swish = swish(x)

plt.figure(figsize=(8, 6))
plt.plot(x, y_relu, label='ReLU', linewidth=2)
plt.plot(x, y_hard_swish, label='Hard Swish', linewidth=2)
plt.plot(x, y_swish, label='Swish', linewidth=2)

plt.axvline(x=0, color='black', linestyle='--', linewidth=0.8)
plt.title('Comparison of Activation Functions')
plt.xlabel('x')
plt.ylabel('f(x)')
plt.legend()
plt.grid(True)
plt.show()
