#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    int reverse(int x)
    {
        //还能优化
        // vector<int> a, m;
        // int b = x >= 0 ? 1 : -1;
        // unsigned int ans = 0;
        // int c = INT32_MAX;
        // bool flag = 0;
        // while (c != 0) {
        //     m.insert(m.begin(), c % 10);
        //     c /= 10;
        // }
        // while (x != 0) {
        //     a.push_back(abs(x % 10));
        //     x /= 10;
        // }
        // if (a.size() < 10) {
        //     for (int i = 0; i < a.size(); i++)
        //         ans = ans * 10 + a[i];
        //     return b * ans;
        // } else {
        //     for (int i = 0; i < 10; i++) {
        //         if (a[i] < m[i])break;
        //         else if (a[i] > m[i]) {
        //             return 0;
        //         }
        //     }
        //     for (int i = 0; i < a.size(); i++)
        //         ans = ans * 10 + a[i];
        //     return b * ans;
        // }
        int ans=0;
        while(x!=0){
            if(ans>INT32_MAX/10||ans<INT32_MIN/10)return 0;
            ans=ans*10+x%10;
            x/=10;
        }
        return ans;
    }
};
int main(void)
{
    int a;
    Solution s;
    cin >> a;
    cout << s.reverse(a);
}