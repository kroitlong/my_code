#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int n, m, ans1, ans2;
vector<int> a;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n;
    f(i, n)cin >> m, a.push_back(m);
    if (a.size() == 1) {
        cout << 1 << " " << 0;
        return 0;
    }
    int p1 = 0, p2 = a.size() - 1;
    while (p1 + 1 != p2) {
        if (a[p1] > a[p2]) {
            a[p1] -= a[p2], p2--, ans2++;
        } else if (a[p1] == a[p2]) {
            p1++, p2--, ans1++, ans2++;
            if (p1 == p2) {
                ans1++;
                cout << ans1 << " " << ans2;
                return 0;
            }
        } else {
            a[p2] -= a[p1], p1++, ans1++;
        }
    }
    ans1++, ans2++;
    cout << ans1 << " " << ans2;
    return 0;
}