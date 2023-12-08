#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int n, m, k, amin, amax, amaxi = 0, amini = 0;
pair<int, int> temp;
vector<int> a;
vector<pair<int, int>> p;
int main()
{
    ios::sync_with_stdio(0);
    cin >> n >> k;
    p.push_back({0, 0});
    f(i, n)cin >> m, a.push_back(m);
    f(i, n) {
        temp.first = i + 1;
        if (temp.second == i)temp.second = i + 1;
        //temp.second = i + 1;
        // amini = i;
        // amaxi = i;
        if (amaxi == i - 1)amaxi = i;
        if (amini == i - 1)amini = i;
        for (int j = temp.second; j < n; j++) {
            amin = min(a[amini], a[j - 1]), amini = (amin == a[j - 1] ? j - 1 : amini);
            amax = max(a[amaxi], a[j - 1]), amaxi = (amax == a[j - 1] ? j - 1 : amaxi);
            if (a[j] - amin <= k && amax - a[j] <= k)temp.second = j + 1;
            else break;
        }
        if (temp.second - temp.first > (p.end() - 1)->second - (p.end() - 1)->first) {
            p.clear();
            p.push_back({0, 0});
            p.push_back(temp);
        } else if (temp.second - temp.first == (p.end() - 1)->second - (p.end() - 1)->first) {
            p.push_back(temp);
        }
    }
    cout << p[1].second - p[1].first + 1 << " " << p.size() - 1 << endl;
    f(i, p.size() - 1) {
        cout << p[i + 1].first << " " << p[i + 1].second << endl;
    }
}