#include<bits/stdc++.h>
using namespace std;
#define F(i,j) for(int i=0;i<j;i++)
pair<int, int> a[2][100002];
int v, b, c, d, e[2], p1, p2, w, t;
bool flag = false;
int main(void)
{//利用p1,p2做指针，通过边界进行输出，直接无需删除或者进行新的拼接
    cin >> v >> b;
    F(i, v)cin >> c >> d, a[c & 1][++e[c & 1]] = {-d, i + 1};
    F(i, 2)sort(a[i] + 1, a[i] + e[i] + 1);
    F(i, 2)F(j, e[i])a[i][j + 1].first += a[i][j].first;
    p1 = min(b, e[1]);//p1指向尾1
    p2 = min(e[0], (b - p1) / 2); //p2指向尾2
    if ((p1 + 2 * p2 + 1) == b) flag = true;
    while (p1 >= 0) {
        if ((t = a[1][p1].first + a[0][p2].first) < w) {
            w = t, ++p2;
            if (flag == true)p1--, flag = false;
            else p1 -= 2;
        } else break;
    }
    --p2, p1 = min(e[1], b - 2 * p2);//回溯
    cout << -1 * w << endl;
    F(i, p2)cout << a[0][i + 1].second << " ";
    F(i, p1 - 1)cout << a[1][i + 1].second << " ";
    if (p1 != 0)
        cout << a[1][p1].second;
}