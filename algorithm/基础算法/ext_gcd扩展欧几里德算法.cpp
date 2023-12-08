#include<bits/stdc++.h>
using namespace std;
#define ll long long
ll a, b, c, x, y, g;
//扩展欧几里得算法
//AX+BY=N*gcd(A,B),N为任意值。
//利用欧几里得算法（辗转相除法）过程中的余数和除数，迭代计算出X和Y。
//已知当B=0时，gcd(A,B)=A,即X=1,Y=0，现在开始推导：当AX(1)+BY(1)=gcd(A,B),
//令BX(2)+(A%B)Y(2)=gcd(B,A%B)==gcd(A,B).化简有AX(1)+BY(1)==AY(2)+B(X2-A/B*Y2)
//即X(1)==Y(2),Y1==(X2-A/B*Y2)，上层可由下层推导出来，且最下层B=0时X=1，Y=0，故可以递推。
void ext_gcd(ll a, ll b, ll &x, ll &y)
{
    if (!b) {
        g = a, x = 1, y = 0;//最底层
        return ;
    }
    ext_gcd(b, a % b, y, x);//这一层的x是下层的y，这一层的y是下层x。下层修改的x和y其实是对应本层的y和x
    y -= a / b * x;//修改前的y表示本层的x，修改前的x表示本层的y。即本层的x已经是下层又该后的y，本层的y则是下层下层x(即本层y-A/B*x(本层的x))，所以这个递归很绕，要仔细分析
}

int main()
{
    cin >> a >> b;
    ext_gcd(a, b, x, y);
    cout << "最大公约数: " << g << endl;
    cout << "满足条件的系数: " << x << " " << y << endl;
}