#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=0;i<j;i++)
#define p(a) ((a)*(a))
#define M_E 2.7182818284590452354
//基于退火模拟的随机搜索算法，逐渐趋近于最优解，一般在题目说明精度时可能用到
//原理：给定一初始温度，给定初始解，给定温度递减系数，给定迭代次数
//从初始温度和初始解开始，带入误差方程，在同一水平温度下找到一稳定
//解，进行下一次迭代（稳定解即在该温度下无法再找到更优解了）即降温
//直到温度到达给定值即可找到近似最优解，相关系数需要自己调试得到，
//注意每次迭代时如果当前温度下内循环没有找到更优解是有概率接受差解
//，概率为e^(-delta/(k*t)),其中delta是两次相邻解的误差的差，k
//是温度衰减系数，t是当前的温度


int x[3], y[3], r[3];
double _sin[3], k = 0.7, T, tx = 0, ty = 0, delta;
double F(double tx, double ty)
{
    //接受扰动后的解，返回误差
    double _r = 0;
    //计算新解与每个点产生的sin的倒数，用于放大误差
    f(i, 3) _sin[i] = sqrt(p(tx - x[i]) + p(ty - y[i])) / r[i];
    //累积误差，得到总的误差
    f(i, 3) _r += p(_sin[i] - _sin[(i + 1) % 3]);
    return _r;
}

int main(void)
{
    srand(time(nullptr));
    f(i, 3)cin >> x[i] >> y[i] >> r[i];
    //初始化解
    f(i, 3)tx += x[i] / 3, ty += y[i] / 3;
    //外层循环进行降温
    for (T = 1; T > 1e-6;) {
        //内层计算该温度下的稳定解
        delta = ((F(tx + T, ty) - F(tx, ty)) + (F(tx - T, ty) - F(tx, ty))
                 + (F(tx, ty + T) - F(tx, ty)) + (F(tx, ty - T) - F(tx, ty))) / 4;
        if (F(tx, ty) > F(tx + T, ty))tx += T;
        else if (F(tx, ty) > F(tx - T, ty))tx -= T;
        else if (F(tx, ty) > F(tx, ty + T))ty += T;
        else if (F(tx, ty) > F(tx, ty - T))ty -= T;
        else if (rand() % (10001) < pow(M_E, (-1 * delta / (k * T))) * 10000) { //如果四个方向都找不到更优解，则一定概率选择一个方向更新解
            int i = rand() % 4;
            if (i == 0)tx += T;
            else if (i == 1)tx -= T;
            else if (i == 2)ty += T;
            else if (i == 3)ty -= T;
        } else T *= k;//直到本次迭代找不到更优解了才进行降温

    }
    if (F(tx, ty) < 1e-5)printf("%.5lf %.5lf", tx, ty);
    return 0;
}