
//处理百万级别数据时超时了（悲)，告诫我们在处理大数据量时不要频繁的遍历和修改容器，选择空间换时间的策略
// #include<bits/stdc++.h>
// using namespace std;
// #define f(i,j) for(int i=0;i<j;i++)
// #define _f(i,j) for(int i=j-1;i<=0;i--)
// int n, m, _max;
// vector<int> hill;
// set<pair<int, int>> s;
//找最大值太费时间了，处理大数据量时直接TO了，再优化一下吧
// int get_max(int i, int j)
// {
//     if (i < j) {
//         return *max_element(hill.begin() + i, hill.begin() + j + 1);
//     } else if (i > j) {
//         auto it1 = max_element(hill.begin() + i, hill.end());
//         auto it2 = max_element(hill.begin(), hill.begin() + j + 1);
//         return *it1 >= *it2 ? *it1 : *it2;
//     } else {
//         return hill[i];
//     }
// }

// int main(void)
// {
//     ios::sync_with_stdio(false);
//     cin >> n;
//     f(i, n)cin >> m, hill.push_back(m);
//     for (int i = 0; i < n; i++)for (int j = 1; j < n; j++) {
//             if (hill[(i + j) % n] > hill[i]) {
//                 s.insert(make_pair(i, (i + j) % n));
//                 s.insert(make_pair((i + j) % n, i));
//                 //_max=0;
//                 break;
//             } else {
//                 if (get_max((i + 1)%n, (i + j) % n) <= *(hill.begin() + (i + j) % n)) {
//                 //if (_max <= *(hill.begin() + (i + j) % n)) {
//                     //_max = *(hill.begin() + (i + j) % n);
//                     s.insert(make_pair(i, (i + j) % n));
//                     s.insert(make_pair((i + j) % n, i));
//                 } else {
//                     continue;
//                 }
//             }
//         }
//     cout << s.size() / 2 << endl;
//     return 0;
// }
// 算了，贴源码看解析去了
// 解析：重构环，化环为链，不改变相对顺序，将最大值（若有多个，选第一个即可）置于链首，再在链末尾添加一个最大值
//对于链的每一个元素i,找到其左边第一个大于i的数的下标和右边第一个大于i的数的下表，分别表示为2对。若在左边遇到
//相同的则same[i]++，对于相同的值情况是一样的，所以后面加上same[i]即可。这里只需要考虑大于的情况，对于小于
//已经包含到相对小的值的大于的处理情况中了。
#include<bits/stdc++.h>
using namespace std;
#define int long long
int n,a[1000005],b[1000005],st[1000005],s[1000005],l,ans,maxi,t;
signed main() {
	cin>>n;
	for(int i=1;i<=n;i++)
		scanf("%lld",a+i),maxi=(a[i]>a[maxi]?i:maxi);
	for(int i=maxi+1;i<=n;i++) 
		b[++t]=a[i];
	for(int i=1;i<maxi;i++)
		b[++t]=a[i];
	for(int i=1;i<=t;i++) {
		while(l&&st[l]<b[i]) 
			ans+=s[l--];
		if(st[l]!=b[i]) 
			st[++l]=b[i],s[l]=0;
		ans+=s[l]++;
	}
	while(l>1) ans+=s[l--];
	cout<<ans+n-1;
	return 0;
}