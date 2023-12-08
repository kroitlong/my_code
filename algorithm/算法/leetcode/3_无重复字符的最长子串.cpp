#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {
        vector<char> a;
        int n = s.size(), l = 0, ans = 0;
        auto it = a.begin();
        for (int i = 0; i < n; i++) {
            if ((it = find(a.begin(), a.end(), s[i])) == a.end()) {
                a.push_back(s[i]);
            } else {
                ans = max(ans, (int)a.size());
                int c=it-a.begin();
                for(int i=0;i<=c;i++){
                    a.erase(a.begin());
                }
                a.push_back(s[i]);
            }
        }
        ans = max(ans, (int)a.size());
        return ans;
    }
};
int main()
{
    string s;
    cin >> s;
    Solution s_;
    cout << s_.lengthOfLongestSubstring(s);
    return 0;
}