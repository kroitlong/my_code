#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    bool isPalindrome(int x)
    {
        if (x < 0)return false;
        if (x == 0)return true;
        int d = int(log10(x)) + 1, a, b;
        if (d == 2 && x % 10 != x / 10)return false;
        a = b = x;
        for (int i = 1; i <= d / 2; i++) {
            if (a / ((int)pow(10, d - i)) != b % 10)return false;
            a = a % (int)pow(10, d - i);
            b = b / 10;
        }
        return true;
    }
};
int main(void)
{
    int a;
    cin >> a;
    Solution s;
    cout << s.isPalindrome(a);
}