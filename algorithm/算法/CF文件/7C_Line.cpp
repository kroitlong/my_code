#include<bits/stdc++.h>
using namespace std;
using ll = long long;
//扩展欧几里得算法
//AX+BY=N*gcd(A,B),N为任意值。
//利用欧几里得算法（辗转相除法）过程中的余数和除数，迭代计算出X和Y。
//迭代公式为初始X0=1,X1=0,Y0=0,Y1=1。X(i+1)=X(i-1)-xI*qI,Y
//同理，这里的qI就是系数A/B，直到余数为零，此时的X,Y就是我们要求的
//X和Y。
ll a, b, c, g, x, y;
void exgcd(ll a, ll b, ll &x, ll &y)
{
    if (!b) {
        g = a, x = 1, y = 0;
        return ;
    }
    exgcd(b, a % b, y, x);
    y -= a / b * x;
}
int main()
{
    cin >> a >> b >> c;
    exgcd(a, b, x, y);
    if (c % g)cout << -1;
    else cout << -c / g *x << ' ' << -c / g *y;
    return 0;
}
