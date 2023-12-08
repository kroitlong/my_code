#include<bits/stdc++.h>
using namespace std;
/*
 @brief 基于欧几里得算法的递归的gcd
*/
int gcd1(int x, int y) {
    if (y == 0)return x;
    else return gcd1(y, x % y);
}
/*
 @brief 基于更相减损法的递归的gcd
*/
int gcd2(int x, int y) {
    if (x == y)return x;
    else {
        if (x > y)return gcd2(y, x - y);
        else return gcd2(x, y - x);
    }
}

int main(void) {
    int x, y;
    while (scanf("%d %d", &x, &y) == 2) {
        cout << gcd1(x, y) << endl;
        cout << gcd2(x, y) << endl;
    }
}