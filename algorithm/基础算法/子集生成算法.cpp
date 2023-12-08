#include<bits/stdc++.h>
using namespace std;
/*
 @param list:原集合
 @param subset:生成的子集
 @param k:当前选择的原集合中值的位置
 @brief 递归生成list集合的所有子集，并通过0/1的形式打印出来。本质上是利用增量法先序遍历二叉树构造子集，
        二叉树的根节点是空集，中间结点是添加元素的过程，叶子节点是构造的子集。
*/
template<typename T>
void _subset(vector<T> &list, vector<int> &subset, int k) {
    if (k == list.size()) {//当所有元素经过选择后，输出得到的子集
        copy(subset.begin(), subset.end(), ostream_iterator<T>(cout, ""));
        cout << endl;
        return;
    } else {//进行选择，每个元素有加入子集和不加入子集两种状态
        //先走加入子集这一选项，再回撤一步选择不加入子集
        subset[k] = 1;
        _subset(list, subset, k + 1);
        //回撤
        subset[k] = 0;
        _subset(list, subset, k + 1);
        return;
    }
}

int main(void) {
    while (1) {
        int n;
        cin >> n;
        vector<int> a(n);
        vector<int> b(n);
        for (auto &x : a) {
            int c;
            cin >> c;
            x = c;
        }
        _subset(a,b,0);
    }
}
