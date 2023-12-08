#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    string convert(string s, int numRows)
    {
        //优化空间大大滴有，再优化一下
        // if (numRows == 1)return s;
        // int ssize = s.size(), k = ssize / (2 * numRows - 2);
        // for (int i = 0; i < (2 * numRows - 2) - (ssize - k * (2 * numRows - 2)); i++) {
        //     s += '#';
        // }
        // int n = s.size() / (2 * numRows - 2) * (numRows - 1), col = 0, i = 0;
        // char tab[numRows][n];
        // string ans;
        // for (int i = 0; i < numRows; i++) {
        //     for (int j = 0; j < n; j++) {
        //         tab[i][j] = 0;
        //     }
        // }
        // for (int m = 0; m < s.size() / (2 * numRows - 2); m++) {
        //     for (int j = 0; j < numRows; j++) {
        //         tab[j][col] = s[i];
        //         i++;
        //     }
        //     col++;
        //     for (int j = numRows - 2; j > 0; j--) {
        //         tab[j][col] = s[i];
        //         i++;
        //         col++;
        //     }
        // }
        // for (int i = 0; i < numRows; i++) {
        //     for (int j = 0; j < col; j++) {
        //         if (tab[i][j] != 0 && tab[i][j] != '#')
        //             ans += tab[i][j];
        //     }
        // }
        // return ans;
        //线性时间，再优化就只有减少判断条件了
        if (numRows >= s.size() || numRows == 1)return s;
        int n = s.size(), r = numRows, i = 0, m = 0;
        string ans(n, ' ');
        while (i < r) {
            for (int j = i; j < n; j += 2 * i) {
                if (s[j] != ' ') {
                    ans[m++] = s[j];
                    s[j] = ' ';
                }
                j += 2 * (r - i - 1);
                if (j < n && s[j] != ' ') {
                    ans[m++] = s[j];
                    s[j] = ' ';
                }
            }
            i++;
        }
        return ans;
    }
};
int main(void)
{
    string a;
    int b;
    getline(cin, a);
    cin >> b;
    Solution s;
    cout << s.convert(a, b);
}