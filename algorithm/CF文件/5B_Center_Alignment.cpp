#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
vector<string> msgs;
string s;
int a, _size, n;
int main(void)
{
    while (getline(cin, s))msgs.push_back(s);
    for (auto it = msgs.begin(); it != msgs.end(); it++) {
        if (_size < it->size())_size = it->size();
    }
    f(i, _size + 2)cout << "*";
    cout << endl;
    //a=0向左靠，a=1向右靠
    for (auto _it = msgs.begin(); _it != msgs.end(); _it++) {
        if (_it->size() % 2 != _size % 2) {
            if (n % 2 == 0)a = 0;
            else a = 1;
            ++n;
        }
        cout << "*";
        f(i, (_size - _it->size() + a) / 2)cout << " ";
        cout << *_it;
        f(i, _size - _it->size() - (_size - _it->size() + a) / 2)cout << " ";
        cout << "*" << endl;
    }
    f(i, _size + 2)cout << "*";
    cout << endl;
}