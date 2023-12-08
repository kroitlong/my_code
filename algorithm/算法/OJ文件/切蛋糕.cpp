//脑子不够，看不懂
#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j-1;i<=0;i--)
/*小度一共可以切K刀，每一刀都是垂直或者水平的，现在小度想知道在切了K刀之后，最重的一块蛋糕最轻的重量是多少。*/
int n, k, m, ans, a[20][20], b[20];
int check(int x)
{
    for (int i = 0; i < 1 << n; i += 2) {//枚举横着切的所有情况，共2^n-1种
        memset(b, 0, sizeof(b));
        int cur = __builtin_popcount(i); //横着切了多少刀，__builtin_popcount(int x)表示x（2）中有多少个1
        for (int j = 0; j < n; j++) {//
_x:
            ;
            if (cur > k)break;
            for (int u = 0, v = 0; u < n; u++) {
                if ((i >> u) & 1)v++;
                if ((b[v] += a[j][u]) > x) {
                    memset(b, 0, sizeof(b));
                    cur++;
                    goto _x;
                }
            }
        }
        if (cur <= k)return 1;
    }
    return 0;
}

int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n >> k;
    f(i, n)f(j, n)cin >> m, a[i][j] = m;
    int L = 0, R = 1e6; //重量的左边界和右边界
    while (L <= R) {
        int mid = L + R >> 1;
        if (check(mid)) ans = mid, R = mid - 1;
        else L = mid + 1;
    }
    cout << ans;
}