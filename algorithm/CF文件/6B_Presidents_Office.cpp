#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int n, m, ans;
char c, b, a[102][102];
set<char> d;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n >> m >> c;
    f(i, n)f(j, m)cin >> b, a[i][j] = b;
    f(i, n) {
        f(j, m) {
            if (a[i][j] == c) {
                if (a[i + 1][j] != c && a[i + 1][j] != 0 && a[i + 1][j] != '.' && d.find(a[i + 1][j]) == d.end()) {
                    d.insert(a[i + 1][j]);
                    ans++;
                }
                if (a[i - 1][j] != c && a[i - 1][j] != 0 && a[i - 1][j] != '.' && d.find(a[i - 1][j]) == d.end()) {
                    d.insert(a[i - 1][j]);
                    ans++;
                }
                if (a[i][j + 1] != c && a[i][j + 1] != 0 && a[i][j + 1] != '.' && d.find(a[i][j + 1]) == d.end()) {
                    d.insert(a[i][j + 1]);
                    ans++;
                }
                if (a[i][j - 1] != c && a[i][j - 1] != 0 && a[i][j - 1] != '.' && d.find(a[i][j - 1]) == d.end()) {
                    d.insert(a[i][j - 1]);
                    ans++;
                }
            }
        }
    }
    cout << ans;
}