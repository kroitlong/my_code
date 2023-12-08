//情况少，考虑暴力，问题是要把所有可能的情况都想到不容易，所以很多问题

#include<bits/stdc++.h>
using namespace std;
#define F(i,j) for(int i=0;i<j;i++)
int b[4][4], n[3], a[3], t;
char c;
int main(void)
{
    F(i, 3)F(j, 3) {
        cin >> c;
        if (c == 'X')b[i + 1][j + 1] = 1;
        else if (c == '0')b[i + 1][j + 1] = 0;
        else b[i + 1][j + 1] = -1;
    }
    F(i, 3)F(j, 3) {
        if ((t = b[i + 1][j + 1]) == -1)n[0]++;
        else if (t == 0)n[1]++;
        else n[2]++;
    }
    //所有胜利的情况，呈’口米‘字形
    if ((t = b[1][1]) == b[1][2] && b[1][2] == b[1][3])a[t + 1]++;
    if ((t = b[1][1]) == b[2][2] && b[2][2] == b[3][3])a[t + 1]++;
    if ((t = b[1][1]) == b[2][1] && b[2][1] == b[3][1])a[t + 1]++;
    if ((t = b[3][3]) == b[3][2] && b[3][2] == b[3][1])a[t + 1]++;
    if ((t = b[3][3]) == b[2][3] && b[2][3] == b[1][3])a[t + 1]++;
    if ((t = b[1][3]) == b[2][2] && b[2][2] == b[3][1])a[t + 1]++;
    if ((t = b[1][2]) == b[2][2] && b[2][2] == b[3][2])a[t + 1]++;
    if ((t = b[2][1]) == b[2][2] && b[2][2] == b[2][3])a[t + 1]++;
    //所有非法的情况，两人同胜，先者超后手两棋，后手超先手一棋，先手赢了后手下，后手赢了先手下
    if ((a[1] == a[2] && a[1] > 0) || n[2] - n[1] > 1
        || n[1] - n[2] > 0 || a[1] > 0 && n[2] - n[1] == 1
        || a[2] > 0 && n[1] == n[2]) {
        cout << "illegal";
        return 0;
    }
    if (a[2] > 0) { //先手胜利
        cout << "the first player won";
        return 0;
    } else if (a[1] > 0) { //后手胜利
        cout << "the second player won";
        return 0;
    }
    //空棋盘
    if (n[0] == 9 || n[0] == 0) {
        cout << "draw";
        return 0;
    }
    //先手下
    if (n[2] == n[1]) {
        cout << "first";
        return 0;
    }
    //后手下
    if (n[2] - n[1] == 1 && n[2] + n[1] < 9) {
        cout << "second";
        return 0;
    }

}