#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int m;
vector<int> a;
int main(void)
{
    f(i, 4)cin >> m, a.push_back(m);
    sort(a.begin(), a.end());
    if (a[1] + a[2] > a[3]) {
        cout << "TRIANGLE";
        return 0;
    } else if (a[1] + a[2] == a[3]) {
        if (a[0] + a[1] > a[2]) {
            cout << "TRIANGLE";
            return 0;
        } else if (a[0] + a[1] <= a[2]) {
            cout << "SEGMENT";
            return 0;
        }
    } else {
        if (a[0] + a[1] > a[2]) {
            cout << "TRIANGLE";
            return 0;
        } else if (a[0] + a[1] == a[2]) {
            cout << "SEGMENT";
            return 0;
        } else {
            cout << "IMPOSSIBLE";
            return 0;
        }
    }
}