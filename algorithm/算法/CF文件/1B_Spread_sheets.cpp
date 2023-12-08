#include<bits/stdc++.h>
using namespace std;
int main(void) {
    int n;
    char ch;
    regex pattern("R\\d+C\\d+");
    cin >> n;
    vector<string> ans;
    for (int i = 0; i < n; i++) {
        string str_a, str_b;
        cin >> str_a;
        if (regex_match(str_a, pattern)) {
            auto it = find(str_a.begin(), str_a.end(), 'C');
            string s_row(str_a.begin() + 1, it);
            string s_col(it + 1, str_a.end());
            int row = stoi(s_row);
            int col = stoi(s_col);
            while (col > 0) {
                int m = col % 26;
                char l;
                if (m == 0) {
                    l = 'Z';
                    col -= 26;
                } else
                    l = 'A' + m - 1;
                str_b.insert(str_b.begin(), l);
                col = col / 26;
            }
            str_b += to_string(row);
        } else {
            auto it = str_a.begin();
            for (; *it >= 'A'; it++)
                ;
            string s_row(it, str_a.end());
            string s_col(str_a.begin(), it);
            int col = 0;
            for (auto _it = s_col.begin(); _it != s_col.end(); _it++) {
                col = col * 26 + static_cast<int>(*_it - 'A' + 1);
            }
            str_b = 'R' + s_row + 'C' + to_string(col);
        }
        ans.push_back(str_b);
    }
    copy(ans.begin(), ans.end(), ostream_iterator<string>(cout, "\n"));
}
