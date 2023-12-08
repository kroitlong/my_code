#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
int n, x, y, z;
vector<pair<int, int>> cat;
vector<int> p;
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n;
    cat.push_back({-1,-1});
    f(i, n)cin >> x >> y, cat.push_back({x, y});
    sort(cat.begin(), cat.end(), [](pair<int, int> k1, pair<int, int> k2) {
        if (k1.first > k2.first)return false;
        else if (k1.first < k2.first)return true;
        else return k1.second > k2.second;
    });
    for (auto it = cat.end() - 1, _it = cat.end() - 1; it != cat.begin(); it--) {
        if (it->first == _it->first) {
            if (it->second > _it->second)it->second = _it->second;
        } else {
            _it = it;
        }
    }
    for (auto it = cat.end() - 1, _it = cat.end() - 1; it != cat.begin(); it--) {
        if (it->first == _it->first)++z;
        if (it->first < _it->first && it->second > _it->second)++z;
        if (it->first < _it->first && it->second <= _it->second) {
            p.push_back(z);
            z = 1;
            _it = it;
        }
    }
    p.push_back(z);
    cout<<*max_element(p.begin(),p.end());
}