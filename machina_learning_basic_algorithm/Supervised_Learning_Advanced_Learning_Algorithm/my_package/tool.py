import copy
import struct
from itertools import combinations_with_replacement
from functools import reduce
from operator import mul


# 导入数据
def load_data(file):
    """
    导入数据
    :param file:数据文件的名字
    :return: x_train,y_train
    """
    _x = []
    x_list = []
    y_list = []
    with open('./' + file, 'r') as file:
        for line in file:
            # 使用逗号分割数据
            parts = line.split(',')
            for i in range(len(parts) - 1):
                _x.append(float(parts[i].strip()))
            # 将分割后的数据添加到不同的列表
            x_list.append(_x)
            y_list.append(float(parts[-1].strip()))
            _x = []
    del _x
    return x_list, y_list


# 扩展线性表达式的数据到高维表达式的数据
def expand_expression(input_x, dim):
    """

    :param input_x: 输入的线性表达式的值，默认为list[x1,x2,...]
    :param dim: 扩展的维度
    :return: 扩展后的线性表达式,即list[1次方的全部组合到n次方的全部组合的线性组合]
    """
    output_x = copy.deepcopy(input_x)
    for i in range(dim - 1):
        result_list = list(combinations_with_replacement(input_x, i + 2))
        for value in result_list:
            temp = reduce(mul, value)
            output_x.append(temp)
        del result_list
    return output_x


# 读取ubyte数据到list
def read_ubyte(path):
    """

    :param path: 传入的文件的路径
    :return: ubyte数据转化成list
    """
    data_list = []
    # 以只读方式打开文件
    file_bin = open(path, 'rb')
    buf = file_bin.read()
    # 获取文件的元数据
    index = 0
    magic = struct.unpack_from('>I', buf, index)
    index += struct.calcsize('>I')
    if magic[0] == 2051:
        images, rows, columns = struct.unpack_from('>III', buf, index)
        im_size = struct.calcsize('>' + f'{rows * columns}' + 'B')
        index += struct.calcsize('>III')
        # 读取数据到list中
        while index < struct.calcsize('>IIII') + images * im_size:
            im = struct.unpack_from('>' + f'{rows * columns}' + 'B', buf, index)
            data_list.append(list(im))
            index += im_size
    elif magic[0] == 2049:
        labels = struct.unpack_from('>I', buf, index)
        index += struct.calcsize('>I')
        while index < struct.calcsize('>II') + labels[0] * struct.calcsize('B'):
            label = struct.unpack_from('B', buf, index)
            data_list.append(label[0])
            index += struct.calcsize('B')

    return data_list
