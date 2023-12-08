#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
int d, _d, sum_time, a, b, t, p[30];
pair<int, int> s[30];
int main(void)
{
    cin >> d >> sum_time, _d = d;
    f(i, d)cin >> a >> b, s[i] = {a, b}, p[i] = a, t += a;
    if (t > sum_time) {
        cout << "NO";
        return 0;
    }
    while (t < sum_time) {
        if (t - s[d - 1].first + s[d - 1].second <= sum_time)
            t = t - s[d - 1].first + s[d - 1].second, p[d - 1] = s[d - 1].second;
        else p[d - 1] = s[d - 1].first + sum_time - t, t = sum_time;
        if (d == 0) {
            cout << "NO";
            return 0;
        }
        --d;
    }
    cout << "YES" << endl;
    copy(p, p + _d, ostream_iterator<int>(cout, " "));
    return 0;
}