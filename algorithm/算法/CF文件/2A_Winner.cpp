#include<bits/stdc++.h>
using namespace std;

int main() {
    //写的屎山代码，还没过，心态爆炸，重写一遍才过，果然有时候map比vector<pair<>>好用
    // vector<pair<string, int>> name_scores;
    // vector<pair<string, int>> name_scores_copy;
    // int n = 0;
    // cin >> n;
    // string name;
    // int score;
    // vector<string> winners;
    // string winner;
    // int winner_nums = 0;
    // int winner_score = INT_MIN;
    // for (int i = 0; i < n; i++) {
    //     cin >> name >> score;
    //     name_scores.push_back({name, score});
    // }
    // for (auto it = name_scores.begin(); it != name_scores.end(); it++) {
    //     auto _it = name_scores_copy.begin();
    //     for (; _it != name_scores_copy.end(); _it++) {
    //         if (it->first == _it->first) {
    //             _it->second += it->second;
    //             break;
    //         }
    //     }
    //     if (_it == name_scores_copy.end())name_scores_copy.push_back({it->first, it->second});
    // }
    // for (auto it = name_scores_copy.begin(); it != name_scores_copy.end(); it++) {
    //     if (it->second > winner_score) {
    //         winner_score = it->second;
    //     }
    // }
    // for (auto it = name_scores_copy.begin(); it != name_scores_copy.end(); it++) {
    //     if (it->second == winner_score)winner_nums++;
    //     winners.push_back(it->first);
    // }
    // if (winners.size() > 1) {
    //     name_scores_copy.clear();
    //     for (auto it = name_scores.begin(); it != name_scores.end(); it++) {
    //         auto _it = name_scores_copy.begin();
    //         for (; _it != name_scores_copy.end(); _it++) {
    //             if (it->first == _it->first) {
    //                 _it->second += it->second;
    //                 break;
    //             }
    //         }
    //         if (_it == name_scores_copy.end()) {
    //             name_scores_copy.push_back({it->first, it->second});
    //             _it = name_scores_copy.end() - 1;
    //         }
    //         if (_it->second >= winner_score && find(winners.begin(), winners.end(), _it->first) != winners.end()) {
    //             winner = _it->first;
    //             break;
    //         }
    //     }
    // } else winner = winners[0];
    // cout << winner << endl;
    map<string, int> name_list;
    vector<pair<string, int>> process;
    vector<string> winners;
    int top_score = INT_MIN;
    string name;
    int n, score;
    cin >> n;
    //记录整个游戏过程，同时记录每一轮游戏结束后所有人的总分。
    for (size_t i = 0; i < n; i++) {
        cin >> name >> score;
        process.push_back({name, score});
        if (name_list.find(name) == name_list.end()) {
            name_list.insert({name, score});
            continue;
        }
        name_list[name] += score;
    }
    //在map中找到最终最大的分数
    for (auto it = name_list.begin(); it != name_list.end(); it++) {
        if (it->second > top_score)top_score = it->second;
    }
    //在winners中存放所有分数为top_score的人
    for (auto it = name_list.begin(); it != name_list.end(); it++) {
        if (it->second == top_score)winners.push_back(it->first);
    }
    //当赢家不止一人时重新遍历游戏过程，决定最终赢家
    if (winners.size() > 1) {
        name_list.clear();
        for (size_t i = 0; i < n; i++) {
            name = process[i].first;
            score = process[i].second;
            if (name_list.find(name) == name_list.end()) {
                name_list.insert({name, score});
            } else {
                name_list[name] += score;
            }
            if (find(winners.begin(), winners.end(), name) != winners.end() && name_list[name] >= top_score) {
                winners[0] = name;
                break;
            }
        }
    }
    cout << winners[0] << endl;

}
