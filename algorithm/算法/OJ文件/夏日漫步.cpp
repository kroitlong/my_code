#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define _f(i,j) for(int i=j;i>0;i--)
int n, m, ans, a[200001], dis[200001], bucket[1000001];
vector<int> seq[200001];
//需要找一个方法一次遍历即可获得下一个等于a[i]的位置(要实现o(n)基本都得用到桶)
// int get(int i)
// {
//     int k;
//     for (k = i + 1; k <= n; k++) {
//         if (a[k] == a[i])break;
//     }
//     return k;
// }
//简单的bfs处理确定到一个节点到另一个结点的最短距离（无权）
int bfs()
{
    int que[200001], tail = 1, cur;
    memset(dis, -1, sizeof(int[200001]));
    dis[1] = 0;
    que[1] = 1;
    for (int u = 1; u <= tail; u++) {
        cur = que[u];
        for (auto v : seq[cur])if (dis[v] == -1) {
                dis[v] = dis[cur] + 1;
                if (v == n)return dis[n];
                que[++tail] = v;
            }
    }
    return -1;//无法到达结点n
}

int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n;
    memset(bucket, -1, sizeof(int[1000001]));
    f(i, n)cin >> m, a[i] = m;
    _f(i, n) {
        if (bucket[a[i]] != -1)seq[i].push_back(bucket[a[i]]);//倒着处理有向边，这样不必担心覆盖问题
        bucket[a[i]] = i;
    }
    for (int i = 1; i < n; i++)seq[i].push_back(i + 1), seq[i + 1].push_back(i);//在处理无向边时，都这样处理即可，注意边界
    cout << bfs();
}