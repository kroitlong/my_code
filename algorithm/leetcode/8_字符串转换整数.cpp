#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    int myAtoi(string s)
    {
        for (auto it = s.begin(); *it == ' ';) {
            s.erase(it);
        }
        long long ans = 0;
        int b = 1;
        //下面的解法毫无美感，给他优化掉
        // bool flag = 0;
        // for (int i = 0; i < s.size(); i++) {
        //     if (s[i] == '+' || s[i] == '-' || (s[i] >= '0' && s[i] <= '9')) {
        //         if (s[i] == '+' || s[i] == '-') {
        //             if (!flag) {
        //                 flag = 1;
        //                 b = (s[i] == '+' ? 1 : -1);
        //                 continue;
        //             } else {
        //                 if (ans > INT32_MAX)return INT32_MAX;
        //                 else if (ans < INT32_MIN) return INT32_MIN;
        //                 else return b * ans;
        //             }
        //         } else {
        //             flag=1;
        //             ans = ans * 10 + s[i] - '0';
        //             if (b * ans > INT32_MAX)return INT32_MAX;
        //             else if (b * ans < INT32_MIN) return INT32_MIN;
        //         }
        //     } else {
        //         if (b * ans > INT32_MAX)return INT32_MAX;
        //         else if (b * ans < INT32_MIN) return INT32_MIN;
        //         else return b * ans;
        //     }
        // }
        // if (b * ans > INT32_MAX)return INT32_MAX;
        // else if (b * ans < INT32_MIN) return INT32_MIN;
        // else return b * ans;
        //现在舒服多了
        b = s[0] == '-' ? -1 : 1;
        int i = (s[0] == '+' || s[0] == '-') ? 1 : 0;
        for (; i < s.size(); i++) {
            if (s[i] < '0' || s[i] > '9')break;
            ans = ans * 10 + s[i] - '0';
            if (b * ans > INT32_MAX)return INT32_MAX;
            else if (b * ans < INT32_MIN) return INT32_MIN;
        }
        return b * ans;
    }
};
int main(void)
{
    string s;
    Solution sb;
    getline(cin, s);
    cout << sb.myAtoi(s);
}