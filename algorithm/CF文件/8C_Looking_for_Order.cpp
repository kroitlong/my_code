#include <bits/stdc++.h>
using namespace std;
//状压dp，很难。。。
int sqr(int x)
{
    return x * x;
}
int x[25], y[25], n, dis[25][25], path[1 << 24];
int dp[(1 << 24)];//dp[i]表示拿个集合i中物品时开销的最小值

int main()
{
    int i, j, l, f, s, cur, maxi, p;
    cin >> x[0] >> y[0] >> n; // 输入起始点的坐标 (x[0], y[0]) 和节点数量 n
    for (i = 1; i <= n; ++i)
        cin >> x[i] >> y[i]; // 输入每个节点的坐标 (x[i], y[i])

    // 计算节点间距离的平方并存储在dis数组中
    for (i = 0; i < n + 1; ++i)
        for (j = 0; j < n + 1; ++j)
            dis[i][j] = sqr(x[i] - x[j]) + sqr(y[i] - y[j]);

    // 初始化dp数组，dp[i]表示状态为i时的最短路径长度，初始值为0x3f3f3f3f
    memset(dp, 0x3f, sizeof(dp));
    dp[0] = 0; // 初始状态，没有节点

    int t = 0x3f3f3f3f;//用于剪枝
    for (i = 0; i < (1 << n) - 1; ++i) { // 枚举所有状态
        if (dp[i] < t) { // 如果dp[i]小于t，继续，当dp[i]>=t则直接进入下一状态，因为dp[i]表示最小开销
            for (j = 0; j < n; ++j) { // 遍历节点
                if (!(i >> j & 1)) { // 如果节点未被访问
                    for (s = j; s < n; ++s) {//s从j开始后向搜索，因为不分顺序，前面每一个已经访问过j了，j就不用反骨来再去访问它们了
                        if (!(i >> s & 1)) { // 如果另一个节点也未被访问
                            int p = i | 1 << j | 1 << s; // 更新状态p
                            int v = dp[i] + dis[0][j + 1] + dis[j + 1][s + 1] + dis[s + 1][0]; // 计算新的路径长度v，即先到j，再到s，再回到0
                            if (dp[p] > v) { // 如果v小于dp[p]，更新dp[p]和path[p]，如果拿两个比拿一个开销少则更新
                                dp[p] = v;
                                path[p] = i;//这里是把拿一个和拿两个的情况合二为一了,path[p]表示到达状态p且使得开销最小的情况下先拿了i结点
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    printf("%d\n", dp[(1 << n) - 1]); // 打印最短路径长度

    cout << 0; // 输出路径起点
    for (i = (1 << n) - 1; i; i = path[i]) { // 回溯路径，从最终状态开始回溯，由path[p]决定路径
        for (j = i ^ path[i]; j; j -= j & -j)
            printf(" %d", int(log(j & -j) / log(2) + 1e-8 + 1)); // 输出节点编号
        printf(" 0"); // 输出路径终点
    }
    putchar('\n');

    return 0;
}

// #include<bits/stdc++.h>
// using namespace std;
// #define ll long long
// #define db double
// #define f(i,j) for(int i=1;i<=j;i++)
// #define p(x) ((x)*(x))
// #define cost(x1,y1,x2,y2) (p(x1-x2)+p(y1-y2))
// unordered_map<int, pair<int, int>> obj;
// pair<int, int> bag;
// vector<int>  dis_bag, seq;
// int x, y, n, ans, temp, min_ = INT32_MAX, min_idx = 0;
// int main(void)
// {
//     cin >> x >> y >> n;
//     bag = {x, y};
//     seq.push_back(0);
//     f(i, n) {
//         cin >> x >> y;
//         obj.emplace(i, make_pair(x, y));
//         dis_bag.push_back(cost(bag.first, bag.second, x, y));
//     }

//新思路是先找距离bag最近的点，再找距离这个点最近的点，看是分别拿开销小还是一起拿开销小，选择一种，然后递归执行到最后。
//这个思路也失败了
// for (int u = 0; u < n;) {
//     auto it = min_element(dis_bag.begin(), dis_bag.end());
//     int gap = it - dis_bag.begin() + 1;
//     f(i, n) {
//         if (i  == gap)continue;
//         if (obj[i].first == INT32_MAX)continue;
//         temp = cost(obj[i].first, obj[i].second, obj[gap].first, obj[gap].second);
//         if (min_ > temp) min_ = temp, min_idx = i;
//     }
//     if (min_idx == -1) {
//         seq.push_back(gap);
//         seq.push_back(0);
//         ans += 2 * dis_bag[gap - 1];
//         u++;
//     } else if (dis_bag[gap - 1] + dis_bag[min_idx - 1] + temp <= 2 * (dis_bag[gap - 1] + dis_bag[min_idx - 1])) {
//         seq.push_back(gap);
//         seq.push_back(min_idx);
//         seq.push_back(0);
//         ans += dis_bag[gap - 1] + dis_bag[min_idx - 1] + temp;
//         u += 2;
//     } else {
//         seq.push_back(gap);
//         seq.push_back(0);
//         seq.push_back(min_idx);
//         seq.push_back(0);
//         ans += 2 * (dis_bag[gap - 1] + dis_bag[min_idx - 1]);
//         u += 2;
//     }
//     dis_bag[gap - 1] = INT32_MAX;
//     dis_bag[min_idx - 1] = INT32_MAX;
//     obj[min_idx].first = INT32_MAX;
//     obj[gap].first = INT32_MAX;
//     min_idx = -1;
//     min_ = INT32_MAX;
// }
//-------------------------------------------------------------------------------------------------
//这个思路不出意料的失败了
// f(i, n) {
//     for (int j = i + 1; j <= n; j++) {
//         temp = cost(obj[i].first, obj[i].second, obj[j].first, obj[j].second);
//         dis.push_back(temp);
//     }
//     dises.push_back(dis);
//     dis.clear();
// }
// int m = 0;
// for (; m < n / 2; m++) {
//     for (int i = 0; i < dises.size() - 1; i++) {
//         for (int j = 0; j < dises[i].size(); j++) {
//             if (dises[i][j] < min_)idx1 = i, idx2 = i + j + 1, min_ = dises[i][j];
//         }
//         if (dises[i][0] == INT32_MAX)continue;
//     }
//     for (int u = 0; u < dises[idx1].size(); u++) {
//         dises[idx1][u] = INT32_MAX;
//     }
//     for (int u = 0; u < dises[idx2].size(); u++) {
//         dises[idx2][u] = INT32_MAX;
//     }
//     for (int u = 0; u < idx1; u++) {
//         dises[u][idx1 - u - 1] = INT32_MAX;
//     }
//     for (int u = 0; u < idx2; u++) {
//         dises[u][idx2 - u - 1] = INT32_MAX;
//     }
//     if (dis_bag[idx1] + dis_bag[idx2] + min_ <= (dis_bag[idx1] + dis_bag[idx2]) * 2) {
//         seq.push_back(idx1 + 1);
//         seq.push_back(idx2 + 1);
//         seq.push_back(0);
//         ans += dis_bag[idx1] + dis_bag[idx2] + min_;
//     } else {
//         seq.push_back(idx1 + 1);
//         seq.push_back(0);
//         seq.push_back(idx2 + 1);
//         seq.push_back(0);
//         ans += 2 * (dis_bag[idx1] + dis_bag[idx2]);
//     }
//     min_ = INT32_MAX;
//     dis_bag[idx1] = INT32_MAX;
//     dis_bag[idx2] = INT32_MAX;
// }
// if (m * 2 != n) {
//     for (int i = 0; i < n; i++) {
//         if (dis_bag[i] != INT32_MAX) {
//             ans += 2 * dis_bag[i];
//             seq.push_back(i + 1);
//             seq.push_back(0);
//             break;
//         }
//     }
// }
// cout << ans << endl;
// for (int i = 0; i < seq.size(); i++)cout << seq[i] << " ";
// return 0;
// }
