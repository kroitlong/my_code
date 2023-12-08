#include<bits/stdc++.h>
using namespace std;
#define ll long long
#define db double
#define f(i,j) for(int i=0;i<j;i++)
//map模拟，代码精简
int x = 105, y = 105;
map<int, int> m{{x << 16 | y, 1}};
char ch;
int main(void)
{
    for (char ch; cin >> ch;) {
        ++m[x << 16 | y - 1];
        ++m[x << 16 | y + 1];
        ++m[(x + 1) << 16 | y];
        ++m[(x - 1) << 16 | y];
        ch == 'R' ? y++ : ch == 'L' ? y-- : ch == 'U' ? x-- : x++;
        if (m[x << 16 | y] > 1)return cout << "BUG", 0;
    }
    cout << "OK";
    return 0;
}
//数组模拟，可过，但是代码比较冗杂且占用空间大
// int map_[210][210], x = 105, y = 105;
// string dir;
// int main(void)
// {
//     f(i, 210)f(j, 210)map_[i][j] = 0;
//     cin >> dir;
//     map_[x][y] = 1;
//     f(i, dir.size()) {
//         if (dir[i] == 'L') {
//             y--;
//             if (map_[x][y - 1] || map_[x - 1][y] || map_[x + 1][y] || map_[x][y]) {
//                 cout << "BUG";
//                 return 0;
//             }
//             map_[x][y] = 1;
//         } else if (dir[i] == 'R') {
//             y++;
//             if (map_[x][y + 1] || map_[x - 1][y] || map_[x + 1][y] || map_[x][y]) {
//                 cout << "BUG";
//                 return 0;
//             }
//             map_[x][y] = 1;
//         } else if (dir[i] == 'U') {
//             x--;
//             if (map_[x][y - 1] || map_[x][y + 1] || map_[x - 1][y] || map_[x][y]) {
//                 cout << "BUG";
//                 return 0;
//             }
//             map_[x][y] = 1;
//         } else {
//             x++;
//             if (map_[x][y - 1] || map_[x][y + 1] || map_[x + 1][y] || map_[x][y]) {
//                 cout << "BUG";
//                 return 0;
//             }
//             map_[x][y] = 1;
//         }
//     }
//     cout << "OK";
// }