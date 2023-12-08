#include<bits/stdc++.h>
using namespace std;
class Solution
{
private:
    unordered_map<int, int> a;
public:
    vector<int> twoSum(vector<int> &nums, int target)
    {
        int idx1,idx2;
        for (int i = 0; i < nums.size(); i++) {
            idx1 = i;
            auto it = a.find(target - nums[i]);
            if(it!=a.end()){
                idx2=it->second;
                break;
            }
            a[num[i]]=i;
        }
        return vector<int>{idx1,idx2};
    }
};