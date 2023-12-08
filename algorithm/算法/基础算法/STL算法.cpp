#include<bits/stdc++.h>
using namespace std;

int main(void) {
    while (1) {
        int n, m;
        cout << "输入向量的元素个数:";
        cin >> n;
        cout << "输入向量:";
        vector<int> a;
        for (size_t i = 0; i < n; i++) {
            cin >> m;
            a.push_back(m);
        }
        int k;
        cout << "输入查找的值:";
        cin >> k;
        //count:返回value出现的次数
        cout << count(a.begin(), a.end(), k) << endl;

        //fill: 将容器某一区间赋值为value
        fill(a.begin(), a.begin() + 3, 10);
        copy(a.begin(), a.end(), ostream_iterator<int>(cout, " "));
        cout << endl;

        //inner_product: 计算两个向量的内积
        cout << "输入第二个向量:";
        vector<int> b;
        for (size_t i = 0; i < n; i++) {
            cin >> m;
            b.push_back(m);
        }
        cout << "两个向量的内积为:" << inner_product(a.begin(), a.end(), b.begin(), 0) << endl;

        //iota: 自动向容器中填充一个递增序列，递增间隔为1，初始值为value
        iota(a.begin(), a.end(), 0);
        copy(a.begin(), a.end(), ostream_iterator<int>(cout, " "));
        cout << endl;

        //is_sorted: 判断容器中的值是否有序,默认升序，可以自定义排序函数
        cout << is_sorted(a.begin(), a.end(), less<int>()) << endl;
        cout << is_sorted(b.begin(), b.end(), [](int a, int b) {
            return a > b;
        }) << endl;

        //mismatch: 返回使a[i]!=b[i]的最小索引i
        auto it = mismatch(a.begin(), a.end(), b.begin());
        cout << *it.first << endl;
        cout << *it.second << endl;

    }
}