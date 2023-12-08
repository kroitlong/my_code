#include<bits/stdc++.h>
using namespace std;
/*
 @param 斐波那契数列的长度
 @return 计算得到的斐波那契数列
 @brief 使用递归计算长度为n+1的斐波那契数列
*/
long long fibonacci(vector<long long> &m, int n) {
    if (n == 0) {
        //m[0] = 0;
        if (*(m.begin()) != 0)
            m.insert(m.begin(), 0);
        return 0;
    } else if (n == 1) {
        if (*(m.begin() + 1) <= 0)
            m.insert(m.begin() + 1, 1);
        //m[1] = 1;
        return 1;
    } else {
        long long ans = fibonacci(m, n - 1) + fibonacci(m, n - 2);
        if (*(m.begin() + n ) <= 0)
            m.insert(m.begin() + n, ans);
        //m[n] = ans;
        return ans;
    }
}

int main(void) {
    int n;
    cin >> n;
    while (n >= 0) {
        vector<long long> m(n);
        fibonacci(m, n);
        for (auto i = 0; i < n; i++) {
            cout << m[i] << " ";
        }
        cout << endl;
        cin >> n;
    }
}