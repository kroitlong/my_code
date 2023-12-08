#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    string longestPalindrome(string s)
    {
        string new_s,ans;
        int l, idx; //只需要记录下标和串长即可
        for (int i = 0; i < s.size() * 2 + 1; i++) { //处理原串，使得串总长为奇数
            if (i % 2 == 0)new_s += '#';
            else new_s += s[i / 2];
        }
        int m = new_s.size(), i, j;
        for (i = 1; i < m; i++) {
            for (j = 1; j <= i; j++) {
                if (new_s[i - j] != new_s[i + j])
                    break;
            }
            if(j-1>l)l=j-1,idx=i;
        }
        for(int i=idx-l;i<=idx+l;i++){
            if(new_s[i]=='#')
            continue;
            ans+=new_s[i];
        }
        return ans;
    }
};
int main()
{
    string s;
    cin >> s;
    Solution S;
    cout << S.longestPalindrome(s);
}