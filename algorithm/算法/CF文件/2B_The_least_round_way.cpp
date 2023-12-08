#include<bits/stdc++.h>
using namespace std;

//-----------------------------------------------------------------------------------
//这个方法会爆内存，算是比较暴力的一种，舍弃。
//-----------------------------------------------------------------------------------

vector<vector<int>> _matrix(1000);
vector<vector<int>> path;
vector<vector<char>> _path;
vector<int> cur_path;
vector<char> cur_path_;
vector<int> num_zero;//每条路径成绩的尾零个数
bool flag = false; //flag表示是否找到路径
void get_factors(int path_i_j, int &num_2, int &num_5) {
    while (path_i_j % 2 == 0) {
        path_i_j /= 2;
        num_2++;
    }
    while (path_i_j % 3 == 0) {
        path_i_j /= 3;
    }
    while (path_i_j % 5 == 0) {
        path_i_j /= 5;
        num_5++;
    }
}

/// @brief 获得一个矩阵从左上角到右下角的全部路径
/// @param cur_path 存储当前的路径上的结点
/// @param cur_path_ 存储当前路径的移动方向次序
/// @param i 前往结点的横坐标
/// @param j 前往结点的纵坐标
/// @param n 矩阵_matrix为(n+1)*(m+1)矩阵
/// @param m 矩阵_matrix为(n+1)*(m+1)矩阵
/// @param dir 此次移动的方向，0为下D，1为右R
void get_path( vector<int> &cur_path,
               vector<char> cur_path_,
               int i, int j, int n, int m, int dir) {
    if (flag )return; //找到路径了，直接返回
    if (i > n || j > m)return; //超出边界则返回
    if (dir == 0) {
        cur_path_.push_back('D');
        cur_path.push_back(_matrix[i][j]);
    } else if (dir == 1) {
        cur_path_.push_back('R');
        cur_path.push_back(_matrix[i][j]);
    }
    get_path( cur_path, cur_path_, i + 1, j, n, m, 0 ); //先向下走
    get_path( cur_path,  cur_path_, i, j + 1, n, m, 1); //再向右走
    //此次走到终点后，将本次的路径加入path和_path中
    if (i == n && j == m) {
        path.push_back(cur_path);
        _path.push_back(cur_path_);
        int num_2 = 0, num_5 = 0;
        bool _flag = false;
        for (auto i = 0; i < cur_path.size(); i++) {
            //遍历路径值，得到该路径上是否有0，路径结点的质因数2的个数和质因数5的个数
            if (cur_path[i] == 0) {
                num_zero.push_back(1);
                _flag = true;
                break;
            }
            get_factors(cur_path[i], num_2, num_5);
        }
        if (!_flag) {
            num_zero.push_back(num_2 >= num_5 ? num_5 : num_2);
        }
        //如果此条路径的尾零为0，则立即结束递归，直接输出此条路径，以减少递归栈的深度
        if (*(num_zero.end() - 1) == 0) {
            flag = true;
        }

    }
    //回溯到当前结点，则移除一次cur_path和cur_path_的尾值
    cur_path.erase(cur_path.end() - 1);
    cur_path_.erase(cur_path_.end() - 1);

}

//完了，老方法爆内存了，要优化一下
int main(void) {
    int n, m;
    cin >> n;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            cin >> m;
            _matrix[i].push_back(m);
        }
    }
    cur_path.push_back(_matrix[0][0]);
    get_path(cur_path, cur_path_, 1, 0, n - 1, n - 1, 0);
    get_path(cur_path, cur_path_, 0, 1, n - 1, n - 1, 1);
    //至此所有路径和移动次序均已存储，接下来找到乘积尾0最少的路径，只要输出一条即可
    //找到第一条尾零最少的路径，输出尾零数和该路径移动序列
    auto it = min_element(num_zero.begin(), num_zero.end());
    int a = it - num_zero.begin();
    cout << *it << endl;
    int i = 0;
    for (; i < _path[a].size(); i++) {
        cout << _path[a][i];
    }
}