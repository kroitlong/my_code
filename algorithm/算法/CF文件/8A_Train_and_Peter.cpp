#include<bits/stdc++.h>
using namespace std;
#define ll long long
#define db double
#define f(i,j) for(int i=0;i<j;i++)
int main()
{
    string s, s1, s2;
    cin >> s >> s1 >> s2;
    bool flag1 = false, flag2 = false;
    if (s.find(s1) != s.npos && s.find(s2, s.find(s1) + s1.size()) != s.npos)
        flag1 = true;
    reverse(s.begin(), s.end());
    if (s.find(s1) != s.npos && s.find(s2, s.find(s1) + s1.size()) != s.npos)
        flag2 = true;
    if (flag1 && flag2)
        cout << "both" << endl;
    else if (flag1)
        cout << "forward" << endl;
    else if (flag2)
        cout << "backward" << endl;
    else
        cout << "fantasy" << endl;
    return 0;
}
