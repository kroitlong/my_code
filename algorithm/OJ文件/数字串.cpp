#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<=j;i++)
int n, k, ans, t;
string num, sub_num;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n >> k >> num;
    int sub_len = n - k;
    for (int i = 0; i <= k ; i++) {
        string cur = num.substr(i, sub_len);
        if (cur > sub_num) {
            sub_num = cur;
            t = i;
        }
    }
    for (int i = 0; i < n;) {
        if (i != t)ans += num[i] - '0', ++i;
        else i += n - k;
    }
    ans += stoi(sub_num);
    cout << ans;
}