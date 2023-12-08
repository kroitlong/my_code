#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
int n, b;
vector<string> msgs;
string temp;
int main(void)
{
    string::iterator it;
    while (getline(cin,temp)) {
        msgs.push_back(temp);
        if (temp[0] == '+')++n;
        else if (temp[0] == '-')--n;
        if ((it = find(temp.begin(), temp.end(), ':')) != temp.end()) {
            b+=(temp.end()-it-1)*n;
        }
    }
    cout<<b;
}
