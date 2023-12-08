#include<bits/stdc++.h>
using namespace std;
/*
 @param 要计算的值
 @return 计算的结果
 @brief 递归的方式计算阶乘，21！已经超过了ll的表达范围
*/
long long factoral1(int n) {
    if (n <= 1)return 1;
    else return n * factoral1(n - 1);
}

/*
 @param 要计算的值
 @return 计算的结果
 @brief 非递归的方式计算阶乘，21！已经超过了ll的表达范围
*/
long long factoral2(int n) {
    long long ans = 1;
    while (n > 0) {
        ans *= n;
        --n;
    }
    return ans;
}


int main(void) {
    int n;
    cin >> n;
    while (n >= 0) {
        cout << factoral2(n) << endl;
        cin >> n;
    }
}