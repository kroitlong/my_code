#include<bits/stdc++.h>
using namespace std;
// class Solution
// {
// public:
// bool isMatch(string s, string p)
// {
//优化正则表达式试试
// int idx = 0;
// char cur = s[0];
// int n = s.size();
// int m = p.size();
// for (int i = 0; i < n; i++) {
//     if (idx >= m)return false;//模式串匹配完了原串还没完，说明不匹配
//     if (p[idx] == '.') {
//         cur = '.';
//         idx++;
//     } else if (p[idx] == '*') {
//         if (s[i] != cur && cur != '.')idx++, i--;//遇到不匹配的时候，回溯进行下一次匹配
//         if (i == n - 1) {
//             idx++;    //如果已经是最后一个字符了，就将idx++;
//             continue;
//         }
//         if (n - i > m - idx)continue; //这里需要判定一下*的匹配次数，
//         else idx++;
//     } else {
//         cur = p[idx];
//         if (cur != s[i] && p[idx + 1] != '*')return false;
//         if (cur != s[i] && p[idx + 1] == '*')i--, idx += 2; //当前字符不匹配且后面是*.说明匹配0个模式字符，因此当前字符需要回溯，进行下一次匹配
//         else idx++;
//     }
// }
// if (idx < m)return false; //模式串没匹配完原串就匹配完了，说明不匹配
// return true;
// }
// };
//官方题解，写不出来
class Solution
{
public:
    bool isMatch(string s, string p)
    {
        int m = s.size();
        int n = p.size();

        auto matches = [&](int i, int j) {
            if (i == 0) {
                return false;
            }
            if (p[j - 1] == '.') {
                return true;
            }
            return s[i - 1] == p[j - 1];
        };

        vector<vector<int>> f(m + 1, vector<int>(n + 1));
        f[0][0] = true;
        for (int i = 0; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (p[j - 1] == '*') {
                    f[i][j] |= f[i][j - 2];
                    if (matches(i, j - 1)) {
                        f[i][j] |= f[i - 1][j];
                    }
                } else {
                    if (matches(i, j)) {
                        f[i][j] |= f[i - 1][j - 1];
                    }
                }
            }
        }
        return f[m][n];
    }
};

int main(void)
{
    string s, p;
    getline(cin, s);
    getline(cin, p);
    Solution _s;
    cout << _s.isMatch(s, p);
}