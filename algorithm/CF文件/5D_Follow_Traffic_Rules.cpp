#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j-1;i<=0;i--)
#define s(a) ((a)*(a))
double a, v, l, d, w;
int main(void)
{
    ios::sync_with_stdio(false);
    cin >> a >> v >> l >> d >> w;
    if (w >= v) {//车速到不了限速
        if (s(v) / 2 / a >= l) { //还没加速到最大值就到了
            printf("%.12lf", sqrt((2 * l) / a));
            return 0;
        } else { //加速到最大值再匀速一段时间
            printf("%.12lf", l / v + v / 2 / a);
        }
    } else {//车速会到限速
        if (d <= s(w) / 2 / a) { //到d前都无法加速到w
            if (s(v) / 2 / a >= l) { //还没加速到最大值就到了
                printf("%.12lf", sqrt((2 * l) / a));
                return 0;
            } else { //加速到最大值再匀速一段时间
                printf("%.12lf", l / v + v / 2 / a);
            }
        } else { //到d前可以加速到w
            if ((d - s(w) / 2 / a) <= (s(v) - s(w)) / a) { //加速到w后不可以加速到v
                if ((s(v) - s(w)) / 2 / a > l - d) { //过了w点不能加速到v
                    printf("%.12lf", 2 * ((sqrt(a * d + s(w) / 2) - w) / a) + w / a + (sqrt(2 * a * (l - d) + s(w)) - w) / a);
                } else { //过了w点可以加速到v
                    printf("%.12lf", 2 * ((sqrt(a * d + s(w) / 2) - w) / a) + w / a + (v - w) / a + (l - d - (s(v) - s(w)) / 2 / a) / v);
                }
            } else { //加速到w后可以加速到v
                if ((s(v) - s(w)) / 2 / a > l - d) { //过了w点不能加速到v
                    printf("%.12lf", (d - s(w) / 2 / a - (s(v) - s(w)) / a) / v + 2 * (v - w) / a + w / a + (sqrt(2 * a * (l - d) + s(w)) - w) / a);
                } else { //过了w点可以加速到v
                    printf("%.12lf", (d - s(w) / 2 / a - (s(v) - s(w)) / a) / v + 2 * (v - w) / a + w / a + (v - w) / a + (l - d - (s(v) - s(w)) / 2 / a) / v);
                }
            }
        }
    }
    return 0;
}