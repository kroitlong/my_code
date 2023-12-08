#include<bits/stdc++.h>
using namespace std;

//基于动态规划求解，在输入完毕后即确定了最短路径，不需要再递归的搜索路径了
int f[1001][1001][2] = {0};
int g[1001][1001][2];
int n, m, x = 0;

void print(int x, int y, int m)
{
    if (x == 1 && y == 1)return;
    if (g[x][y][m]) {
        print(x - 1, y, m);
        cout << "D";
    } else {
        print(x, y - 1, m);
        cout << "R";
    }
}

int main(void)
{
    cin >> n;
    for (size_t i = 2; i <= n; i++) {
        f[i][0][0] = f[i][0][1] = f[0][i][0] = f[0][i][1] = INT_MAX;
    }
    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= n; j++) {
            cin >> m;
            if (!m)x = i;//得到0在第几行
            else {
                while (m % 2 == 0) {
                    m /= 2;
                    f[i][j][0]++;
                }
                while (m % 5 == 0) {
                    m /= 5;
                    f[i][j][1]++;
                }
            }
            for (size_t k = 0; k < 2; k++) {
                if (g[i][j][k] = f[i - 1][j][k] < f[i][j - 1][k])
                    f[i][j][k] += f[i - 1][j][k];
                else f[i][j][k] += f[i][j - 1][k];
            }

        }
    }
    m = f[n][n][0] > f[n][n][1];
    if (x && f[n][n][m] > 1) {
        //路径存在0时
        cout << "1" << endl;
        for (auto i = 2; i <= x; i++) {
            cout << "D";
        }
        for (auto i = 2; i <= n; i++) {
            cout << "R";
        }
        for (auto i = x + 1; i <= n; i++) {
            cout << "D";
        }
    } else { //路径不存在0时
        cout << f[n][n][m] << endl;
        print(n, n, m);
    }
}