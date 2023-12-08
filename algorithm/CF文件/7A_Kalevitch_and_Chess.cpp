#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define f_(i,j) for(int i=0;i<n;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
char a[9][9], m;
int row, ans;
bool check = false;
int main(void)
{
    f(i, 8)f(j, 8)cin >> m, a[i][j] = m;
    f(i, 8) {
        f(j, 8) {
            if (a[i][j] == 'B')++row;
        }
        if (row == 8)ans++;
        else if (!check) ans += row, check = true;
        row = 0;
    }
    cout << ans;
}