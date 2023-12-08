#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)

//输入n个结点，然后输入m条边，利用bfs和dfs从s开始遍历图，图使用邻接表存储,假设最多有1e4个结点
int n, m, s;
vector<int> g[10001];
int dis[2][10001];
void dfs(int dis[], int s)
{
    dis[s] = 1;
    for (auto v : g[s])if (dis[v] == -1) {
            cout << v << " ";
            dfs(dis, v);
        }
}
void bfs(int dis[], int s)
{
    int que[10001], tail = 1;
    que[1] = s;
    dis[s] = 1;
    for (int i = 1; i <= tail; i++) {
        int cur = que[i];
        for (auto v : g[cur])if (dis[v] == -1) {
                dis[v] = 1;
                cout << v << " ";
                que[++tail] = v;
            }
    }
}

int main(void)
{
    ios::sync_with_stdio(0);
    cin >> n >> m >> s;
    f(i, m) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        g[v].push_back(u);
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < g[i].size(); j++)cout << g[i][j] << " ";
        cout << endl;
    }
    memset(dis[0], -1, sizeof(int[10001]));
    memset(dis[1], -1, sizeof(int[10001]));
    dfs(dis[0], s);
    cout << endl;
    bfs(dis[1], s);
    return 0;
}
/*
6 6 1
1 2
1 3
1 4
3 5
5 6
4 6
*/