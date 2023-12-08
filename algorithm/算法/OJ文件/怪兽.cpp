#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<=j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int p, q, a, b, c, ans1 = -1, ans2 = -1;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> p >> q >> a >> b >> c;
//暴力，可过
//--------------------------------------------------------------//
    f(i, p) {                                                //
        f(j, p - i) {                                        //
            if ((i * c + j * b + (p - i - j) * a) == q) {    //
                ans1 = i;                                    //
                goto x;                                      //
            }                                                //
        }                                                    //
    }                                                        //
x:                                                           //
    ;                                                        //
    _f(i, p) {                                               //
        f(j, p - i) {                                        //
            if ((i * c + j * b + (p - i - j) * a) == q) {    //
                ans2 = i;                                    //
                goto y;                                      //
            }                                                //
        }                                                    //
    }                                                        //
y:                                                           //
    ;                                                        //
//--------------------------------------------------------------//
//对等式化简后的枚举--有一个案列没过，不知道为啥
    // int m = b - a, n = c - a, t;
    // f(i, p) {
    //     if (fmod(static_cast<double>(q - a * p - m * i), n) == 0.0 && (t = (q - a * p - m * i) / n) + i <= p && t >= 0) {
    //         ans2 = t;
    //         break;
    //     }
    // }
    // _f(i, p) {
    //     if (fmod(static_cast<double>(q - a * p - m * i), n) == 0.0 && (t = (q - a * p - m * i) / n) + i <= p && t >= 0) {
    //         ans1 = t;
    //         break;
    //     }
    // }
    if (ans1 < 0 || ans2 < 0)cout << -1;
    else cout << ans1 << " " << ans2;
}