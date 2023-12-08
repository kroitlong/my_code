#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j-1;i<=0;i--)
//暴力超时
int t, p, k;
// int get(int m, int n) //m个糖果+n张糖纸最终可以得到多少糖果
// {
//     if (n < p)return m;
//     return m + get(n / p, n / p + n % p);
// }
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> t;
    f(i, t) {
        cin >> p >> k;
        if (p == 1) {
            if (k == 0)cout << 0 << endl;
            else
                cout << 1 << endl;
            continue;
        }
        //倒推，要k颗糖果，说明在得到第k颗糖果后一共获得了k张糖纸，由于获得第k颗糖果后这张糖纸不能用
        //所以实际获得k颗糖果能换的糖纸只有k-1张，故能换的最大糖果数是（k-1)/p,故答案为k-(k-1)/p;
        int m = k - (k - 1) / p;
        // int m, n, l = 0, r = k;
        // m = l + r >> 1;
        // while ((n = get(m, m)) != k) {
        //     if (n > k)r = m - 1;
        //     else l = m + 1;
        //     m = l + r >> 1;
        // }

        cout << m << endl;
    }
}
