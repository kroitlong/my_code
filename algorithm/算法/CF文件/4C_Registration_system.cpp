#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
int n, _l[100000];
vector<string> l;
string name;
int main(void)
{
    cin >> n;
    auto it = l.begin();
    f(i, n) {
        cin >> name;
        if ((it = find(l.begin(), l.end(), name)) == l.end())
            l.push_back(name), cout << "OK" << endl;
        else cout << name << _l[it-l.begin()]+1 << endl,++_l[it-l.begin()];
    }
}