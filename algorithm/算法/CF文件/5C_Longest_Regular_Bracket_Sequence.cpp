#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j-1;i>=0;i--)
int c[1000001], vis[1000001], cun, _max, idx_0 = -1;
string s;

int main(void)
{
    ios::sync_with_stdio(false);
    cin >> s;
    f(i, s.size())if (s[i] == '(')cun++;
    else if (cun > 0) c[i] = 1, --cun;
    cun = 0;
    _f(i, s.size())if (s[i] == ')')cun++;
    else if (cun > 0)c[i] = 1, --cun;
    f(i, s.size())if (c[i] == 1)vis[i] = i - idx_0;
    else vis[i] = 0, idx_0 = i;
    cun = 0;
    f(i, s.size())if (vis[i] > _max)_max = vis[i];
    if (_max == 0)cout << 0 << " " << 1;
    else {
        f(i, s.size())if (vis[i] == _max)++cun;
        cout << _max << " " << cun;
    }
}