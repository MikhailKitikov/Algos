#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
#include <climits>

int solve(int target_len, int n_strings, int alphabet_size, 
    std::vector<std::string>& patterns) {

    const int MODULE = 1000000007;
    const int MAX_SUM_LENGTH = 1010;

    // construct automaton
    
    int states_cnt = 1;
    std::vector<int> suffix_links(MAX_SUM_LENGTH, INT_MAX);
    std::vector<char> is_terminal(MAX_SUM_LENGTH, 0);
    std::vector<std::vector<int>> trie(MAX_SUM_LENGTH, 
        std::vector<int>(alphabet_size, INT_MAX));
    std::vector<std::vector<int>> next_try(MAX_SUM_LENGTH,
        std::vector<int>(alphabet_size, INT_MAX));
    
    for (int i = 0; i < n_strings; ++i) {
        int vert = 0;
        for (auto symb : patterns[i]) {
            int symb_code = symb - 'a';
            if (trie[vert][symb_code] == INT_MAX) {
                trie[vert][symb_code] = states_cnt++;
            }
            vert = trie[vert][symb_code];
        }
        is_terminal[vert] = 1;
    }

    suffix_links[0] = 0;

    std::queue<int> queue;

    for (int symb_code = 0; symb_code < alphabet_size; ++symb_code) {
        if (trie[0][symb_code] == INT_MAX) {
            trie[0][symb_code] = 0;
        } else {
            suffix_links[trie[0][symb_code]] = 0;
            next_try[0][symb_code] = 0;
            queue.push(trie[0][symb_code]);
        }
    }

    while (!queue.empty()) {
        int vert = queue.front();
        queue.pop();
        for (int symb_code = 0; symb_code < alphabet_size; ++symb_code) {
            if (trie[vert][symb_code] == INT_MAX) {
                continue;
            }                
            int suff_link = suffix_links[vert];
            while (trie[suff_link][symb_code] == INT_MAX) {
                suff_link = suffix_links[suff_link];
            }
            suffix_links[trie[vert][symb_code]] = trie[suff_link][symb_code];
            next_try[vert][symb_code] = trie[suff_link][symb_code];
            is_terminal[trie[vert][symb_code]] |= is_terminal[trie[suff_link][symb_code]];
            queue.push(trie[vert][symb_code]);
        }
    }

    auto suffix_transition = [&](int vert, int symb_code) -> int {
        while (trie[vert][symb_code] == INT_MAX) {
            vert = suffix_links[vert];
        }
        return trie[vert][symb_code];
    };
    
    // calculate dp

    std::vector<int> dp_prev(states_cnt);
    std::vector<int> dp_new(states_cnt);

    for (int i = 0; i < states_cnt; ++i) {
        dp_prev[i] = 1 - is_terminal[i];
    }

    for (int len = 0; len < target_len; ++len) {
        std::fill(dp_new.begin(), dp_new.end(), 0);
        for (int vert = 0; vert < states_cnt; ++vert) {
            if (is_terminal[vert]) {
                continue;
            }
            for (int symb_code = 0; symb_code < alphabet_size; ++symb_code) {
                if (trie[vert][symb_code] != INT_MAX) {
                    int to_check = suffix_links[trie[vert][symb_code]];
                    if (!is_terminal[to_check]) {
                        dp_new[vert] = (dp_new[vert] + 
                            dp_prev[trie[vert][symb_code]]) % MODULE;
                    }                    
                } else {
                    if (next_try[vert][symb_code] == INT_MAX) {
                        next_try[vert][symb_code] = suffix_transition(vert, symb_code);
                    }
                    int to = next_try[vert][symb_code];
                    dp_new[vert] = (dp_new[vert] + dp_prev[to]) % MODULE;
                }
            }
        }
        dp_new.swap(dp_prev);
    }
    return dp_prev[0];
}

int main() {
    int target_len, n_strings, alphabet_size;
    std::cin >> target_len >> n_strings >> alphabet_size;

    std::vector<std::string> patterns(n_strings);

    for (int i = 0; i < n_strings; ++i) {
        std::cin >> patterns[i];
    }

    int ans = solve(target_len, n_strings, alphabet_size, patterns);
    std::cout << ans;

    return 0;
}
