#include<bits/stdc++.h>
using namespace std;
class Solution
{
public:
    double findMedianSortedArrays(vector<int> &nums1, vector<int> &nums2)
    {
        //偶数个中位数在(m+n)/2-1到(m+n)/2之间，奇数个在(m+n)/2
        //直接组合数组，然后输出吧。
        vector<int> nums3;
        int cnt = 0, m = nums1.size(), n = nums2.size(), p1 = 0, p2 = 0;
        double ans;
        while (cnt < (m + n) / 2 + 1) {
            if (p1 == m) {
                int i = nums3.size();
                while (i < (m + n) / 2 + 1) {
                    nums3.push_back(nums2[p2]);
                    p2++;
                    i++;
                }
                break;
            } else if (p2 == n) {
                int i = nums3.size();
                while (i < (m + n) / 2 + 1) {
                    nums3.push_back(nums1[p1]);
                    p1++;
                    i++;
                }
                break;
            }
            if (nums1[p1] <= nums2[p2]) {
                nums3.push_back(nums1[p1]);
                p1++, cnt++;
            } else {
                nums3.push_back(nums2[p2]);
                p2++, cnt++;
            }
        }
        int v = nums3.size();
        if ((m + n) % 2 == 0) {
            ans = (double)(nums3[v - 1] + nums3[v - 2]) / 2;
        } else {
            ans = (double)nums3[v - 1];
        }
        return ans;
    }
};
vector<int> nums1, nums2;
int main(void)
{
    int m, n, x;
    Solution s;
    cin >> m >> n;
    for (int i = 0; i < m; i++) {
        cin >> x;
        nums1.push_back(x);
    }
    for (int i = 0; i < n; i++) {
        cin >> x;
        nums2.push_back(x);
    }
    cout << s.findMedianSortedArrays(nums1, nums2);
}