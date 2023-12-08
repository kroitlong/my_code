#include<bits/stdc++.h>
using namespace std;
#define f(i,j) for(int i=1;i<=j;i++)
#define f_(i,j) for(int i=0;i<j;i++)
#define _f(i,j) for(int i=j;i>=0;i--)
int t, m, n, seq = 1, all = 0;
string order;
vector<pair<int, pair<int, int>>> mem;
vector<pair<int, int>> block; //记录空闲空间的初始位置及其大小
int main(void)
{
    cin >> t >> m;
    block.push_back({1, m});
    auto it = block.end();
    auto it1 = mem.end();
    cin.ignore();
    f_(i, t) {
        getline(cin, order);
        if (order[0] == 'a') {
            n = stoi(string(order.begin() + 6, order.end()));
            it = find_if(block.begin(), block.end(), [](pair<int, int> k) {
                return k.second >= n;
            });
            if (it == block.end())cout << "NULL" << endl;
            else {
                cout << seq << endl;
                //选择在构造mem时对其进行排序
                //mem.push_back({seq, {it->first, n}});
                it1 = find_if(mem.begin(), mem.end(), [it](pair<int, pair<int, int>> k) {
                    return k.second.first > it->first;
                });
                mem.insert(it1, {seq, {it->first, n}});
                seq++;
                if (it->second == n)block.erase(it), it = block.end();
                else {
                    it = block.insert(it, {it->first + n, it->second - n}) + 1;
                    block.erase(it), it = block.end();
                }
            }
        } else if (order[0] == 'e') {
            n = stoi(string(order.begin() + 6, order.end()));
            if ((it1 = find_if(mem.begin(), mem.end(), [](pair<int, pair<int, int>> k) {
            return k.first == n;
        })) == mem.end())
            cout << "ILLEGAL_ERASE_ARGUMENT" << endl;
            else {
                it = find_if(block.begin(), block.end(), [it1](pair<int, int> k) {
                    return k.first > it1->second.first;
                });
                if (it == block.end()) {
                    block.push_back({it1->second.first, it1->second.second});
                } else {
                    //考虑合并空闲空间
                    if (it1->second.first + it1->second.second != it->first
                        && (it - 1)->first + (it - 1)->second != it1->second.first)
                        it = block.insert(it, {it1->second.first, it1->second.second});
                    else {
                        if (it1->second.first + it1->second.second == it->first)
                            it = block.insert(it, {it1->second.first, it1->second.second + it->second}), block.erase(it + 1);
                        if ((it - 1)->first + (it - 1)->second == it1->second.first)
                            it = block.insert(it, {(it - 1)->first, (it - 1)->second + it1->second.second}), block.erase(it - 1);
                    }
                }
                mem.erase(it1), it1 = mem.end();
            }
        } else if (order[0] == 'd') {
            if (mem.size() != 0) {
                //不可改变原有顺序，故根据其起始下标进行排序或选择在构造mem时进行排序，前者比较方便,后者更加直观
                // sort(mem.begin(), mem.end(), [](pair<int, pair<int, int>> k1, pair<int, pair<int, int>> k2) {
                //     return k1.second.first < k2.second.first;
                // });
                mem[0].second.first = 1;
                all = mem[0].second.second;
                f(i, mem.size() - 1) {
                    mem[i].second.first = mem[i - 1].second.first + mem[i - 1].second.second;
                    all += mem[i].second.second;
                }
                block.clear();
                block.push_back({(mem.end() - 1)->second.first + (mem.end() - 1)->second.second,
                                 m - all});
                int x = 0;
            } else {
                //do nothing
            }
        }
    }
}