#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
string s;
set<pair<int, int>> seq;
int _count, a, b;
long long c;

//动态规划+贪心+回溯。这里每次处理一个？，将其设置为），再查看当前位
//置（和）的数量，允许（比）多，但是不允许）比（多，多则不存在
//合法序列。这样每次处理都能保证当前位置前具有最低开销，处理过
//程即将前面更改过的？且最能降低开销的更改为（以保证在序列合法
//的前提下开销最小。

int main(void)
{
    cin >> s;
    f(i, s.size()) {
        if (s[i] == '(')++_count;
        else if (s[i] == ')')--_count;
        else {
            s[i] = ')', --_count, cin >> a >> b, c += b, seq.insert({-b + a, i});
        }
        while (_count < 0) {
            if (seq.empty()) {
                cout << -1;
                return 0;
            }
            c += seq.begin()->first, _count += 2, s[seq.begin()->second] = '(', seq.erase(seq.begin());
        }
        if (_count < 0)break;
    }

    if (_count != 0)cout << -1;
    else cout << c << endl << s;
}