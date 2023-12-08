#include <bits/stdc++.h>
using namespace std;
const long double eps = 1e-4;
const long double PI = acos(-1.0);

long double sq(long double x) {
    return x * x;
}
long double gcd(long double a, long double b) {
    if (a < eps) return b;
    if (b < eps) return a;
    return gcd(b, fmod(a, b));
}

int main() {
    long double x1, y1, x2, y2, x3, y3; cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    long double a = sqrt(sq(x1 - x2) + sq(y1 - y2));
    long double b = sqrt(sq(x2 - x3) + sq(y2 - y3));
    long double c = sqrt(sq(x1 - x3) + sq(y1 - y3));
    //海伦公式
    long double s = (a + b + c) / 2;
    long double A = sqrt(s * (s - a) * (s - b) * (s - c));
    long double r = a * b * c / 4.0 / A;
    //计算圆心角
    long double Alpha = acos((2 * sq(r) - sq(a)) / (2.0 * sq(r)));
    long double Beta = acos((2 * sq(r) - sq(b)) / (2.0 * sq(r)));
    long double Gamma = 2 * PI - Alpha - Beta;

    long double sta = gcd(Alpha, gcd(Beta, Gamma)); //取最大公约数即为中心角
    long double S_min = sin(sta) * (PI / sta) * sq(r);
    cout << fixed << setprecision(8) << S_min << endl;
    return 0;
}