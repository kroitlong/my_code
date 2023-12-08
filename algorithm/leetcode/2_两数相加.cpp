
// Definition for singly-linked list.
#include<bits/stdc++.h>
using namespace std;
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
class Solution
{
public:
    ListNode *addTwoNumbers(ListNode *l1, ListNode *l2)
    {
        int b = 0;
        ListNode *ans = new ListNode;
        ListNode *p = ans, *p1 = l1, *p2 = l2;
        while (p1 != nullptr || p2 != nullptr || b != 0) {
            ListNode *a = new ListNode;
            // if (p1 == nullptr && p2 != nullptr)
            //     a->val = (p2->val + b) % 10, b = (p2->val + b) / 10;
            // else if (p1 != nullptr && p2 == nullptr)
            //     a->val = (p1->val + b) % 10, b = (p1->val + b) / 10;
            // else if (p1 != nullptr && p2 != nullptr)
            //     a->val = (p1->val + p2->val + b) % 10, b = (p1->val + p2->val + b) / 10;
            // else a->val = b % 10, b = b / 10;
            int val1 = (p1 != nullptr) ? p1->val : 0;
            int val2 = (p2 != nullptr) ? p2->val : 0;
            a->val = (val1 + val2 + b) % 10, b = (val1 + val2 + b) / 10;
            p->next = a;
            p = a;
            if (p1 != nullptr)p1 = p1->next;
            if (p2 != nullptr)p2 = p2->next;
        }
        return ans->next;
    }
};
int main(void)
{
    Solution s;
    ListNode *l1 = new ListNode;
    ListNode *l2 = new ListNode;
    ListNode *p1 = l1, *p2 = l2;
    int m, n, x;
    cin >> m >> n;
    for (int i = 0; i < m; i++) {
        cin >> x;
        ListNode *a = new ListNode;
        a->val = x;
        p1->next = a;
        p1 = a;
    }
    for (int i = 0; i < n; i++) {
        cin >> x;
        ListNode *a = new ListNode;
        a->val = x;
        p2->next = a;
        p2 = a;
    }
    ListNode *ans = s.addTwoNumbers(l1, l2), *p3;
    p3 = ans->next;
    while (p3 != nullptr) {
        cout << p3->val << " ";
        p3 = p3->next;
    }
    return 0;
}