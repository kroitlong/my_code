#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j-1;i<=0;i--)
int te, fe, s, t, f, n, m;
vector<int> son[40010];//采用邻接表存储图
int dis[3][40010];
int que[40010];//用来存储已经访问过的结点
//计算结点i到图中每个结点的最短距离，无法到达则设置为-1
void calc(int dis[], int s)
{//基于邻接表的bfs模板，关键是下面两个循环，使用一个que来存储访问过的结点，
 //对每个结点，遍历其邻接表，若没有访问过则设置访问标志
    int tail = 1;//表示访问过的最后一个结点
    memset(dis, -1, sizeof(int[40010]));
    dis[s] = 0;
    que[1]=s;
    for (int i = 1; i <= tail; i++) {
        int cur = que[i];
        for (auto v :son[cur])if(dis[v]==-1) {
            dis[v]=dis[cur]+1;
            que[++tail]=v;
        }   
    }
}
int main(void)
{
    cin >> te >> fe >> s;
    cin >> t >> f >> n >> m;
    int u, v;
    f(i, m)cin >> u >> v, son[u].push_back(v), son[v].push_back(u);
    calc(dis[0], t);
    calc(dis[1], f);
    calc(dis[2], n);
    long long ans = LONG_LONG_MAX;
    for (int i = 1; i <= n; i++) {
        if (dis[0][i] != -1 && dis[1][i] != -1 && dis[2][i] != -1) {
            ans = min(ans, 1ll * te * dis[0][i] + 1ll * fe * dis[1][i] + 1ll * (te + fe - s) * dis[2][i]);
        }
    }
    cout << (ans == LONG_LONG_MAX ? -1 : ans);
}