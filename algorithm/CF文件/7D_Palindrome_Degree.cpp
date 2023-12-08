#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define f_(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
string s;
int dp[5000001], l, r, e = 1;
long long ans;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> s;
    memset(dp, 0, sizeof(int[5000001]));
    f_(i, s.size()) {
        /*字符哈希映射，快速判断回文，l表示正向哈希，r表示反向哈希，正反一致表示回文
        只要找到一个合适的哈希映射，使得正向哈希和反向哈希相同就能进行回文判断,一下
        是一种方式*/
        // l = l * 107 + s[i];//这里的正向哈希就是增加前面的权值
        // r += s[i] * e;//这里的反向哈希就是增加后面的权值
        // e *= 107;//e只是为了减少计算幂的一个工具
        /*下面也是一种方式，即将正向哈希和反向哈希反过来，本质上都是hash(i)=hash[i-1]+s[i]*pow(p,i),其中p为素数*/
        l += s[i] * e;
        r = r * 107 + s[i];
        e *= 107;
        dp[i] = (l == r) ? dp[(i - 1) / 2] + 1 : 0;
        ans += dp[i];
    }
    cout << ans;
}