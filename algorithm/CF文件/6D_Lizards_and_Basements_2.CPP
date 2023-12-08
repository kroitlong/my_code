#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int n, a, b, m, v = 1e9, N[101];
vector<int> ans, temp;
//dfs是为了找到满足条件的各种情况，都尝试着去搜索，判优是在最后才执行的。不要一开始就陷入找到最优的泥潭中了
void dfs(int p, int k)//搜索p号结点，当前攻击次数为k
{
    if (k >= v)return;
    if (p == n) {
        //搜索到最后一个结点了，当满足攻击次数减小且使得最后一个结点的能被消灭才更新
        if (N[p] < 0) {
            ans.clear();
            ans = temp;
            v = k;
        }
        return;
    }
    for (int i = 0; i <= max(N[p] / a + 1, max(N[p - 1] / b + 1, N[p + 1] / b + 1)); i++) {
        //在保证最左边的能打死的前提下，尝试攻击目标p的各种次数，每种次数对应一个状态，进行dfs
        if (N[p - 1] - i * b >= 0)continue;
        N[p - 1] -= i * b;
        N[p] -= i * a;
        N[p + 1] -= i * b;
        f(j, i)temp.push_back(p);
        dfs(p + 1, k + i);
        N[p - 1] += i * b;
        N[p] += i * a;
        N[p + 1] += i * b;
        f(j, i)temp.pop_back();
    }
}
int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n >> a >> b;
    f(i, n)cin >> m, N[i] = m;
    dfs(2, 0);
    cout << v << endl;
    //copy(ans.begin(), ans.end(), ostream_iterator<int>(cout, " "));
    f(i, v)cout << ans[i-1] << " ";
    cout << endl;
}