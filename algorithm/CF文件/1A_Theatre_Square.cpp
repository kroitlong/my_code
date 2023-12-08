#include<bits/stdc++.h>
using namespace std;

int main(void) {
    int n, m, a;
    cin >> n >> m >> a;
    cout << static_cast<long long>((n - 1) / a + 1)*((m - 1) / a + 1);
}