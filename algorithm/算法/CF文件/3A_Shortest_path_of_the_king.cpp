#include<bits/stdc++.h>
using namespace std;
string a, b, c;
vector<string> path;
void print_path(string &a, string &b)
{
    if (a == b)return;
    if (a[0] < b[0]) {
        ++a[0];
        path.push_back("R");
    } else if (a[0] > b[0]) {
        --a[0];
        path.push_back("L");
    } else {
        path.push_back("");
    }
    if (a[1] < b[1]) {
        ++a[1];
        path.push_back("U");
    } else if (a[1] > b[1]) {
        --a[1];
        path.push_back("D");
    } else {
        path.push_back("");
    }
    print_path(a, b);
}

int main(void)
{
    cin >> a >> b;
    print_path(a, b);
    cout << path.size() / 2 << endl;
    for (auto i = 0; i < path.size(); i += 2) {
        c = path[i] + path[i + 1];
        cout << c << endl;
    }
}
