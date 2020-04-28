#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <climits>


class Heap {
public:
    Heap() {}

    void HeapifyDown() {
        size_t pos = 0;
        while (pos <= heap.size()) {
            size_t min_child = 2 * pos + 1;
            if (min_child >= heap.size()) {
                return;
            }
            if (2 * pos + 2 < heap.size() && heap[2 * pos + 2].first < heap[2 * pos + 1].first) {
                min_child = 2 * pos + 2;
            }
            if (heap[pos].first > heap[min_child].first) {
                std::swap(heap[pos], heap[min_child]);
                pos = min_child;
                continue;
            }
            return;
        }
    }

    void HeapifyUp() {
        size_t pos = heap.size() - 1;
        while (pos >= 1) {
            size_t parent = (pos - 1) / 2;
            if (heap[pos].first < heap[parent].first) {
                std::swap(heap[pos], heap[parent]);
                pos = parent;
                continue;
            }
            return;
        }
    }

    void Add(int item, size_t index) {
        heap.emplace_back(item, index);
        HeapifyUp();
    }

    std::pair<int, size_t> GetMin() {
        return heap[0];
    }

    void Modify(int val) {
        int saved = heap[0].first;
        heap[0].first = val;
        if (val <= saved) {
            return;
        }
        HeapifyDown();
    }

private:
    std::vector<std::pair<int, size_t>> heap;
};


int main() {

    size_t num_seq, seq_len;
    std::cin >> num_seq >> seq_len;
    std::vector<std::vector<int>> data(num_seq, std::vector<int>(seq_len));
    for (size_t i = 0; i < num_seq; ++i) {
        for (size_t j = 0; j < seq_len; ++j) {
            std::cin >> data[i][j];
        }
    }

    Heap heap;
    for (size_t i = 0; i < num_seq; ++i) {
        heap.Add(data[i][0], i);
    }
    std::vector<size_t> position(num_seq, 1);

    for (size_t i = 0; i < seq_len * num_seq; ++i) {
        auto min_pair = heap.GetMin();
        std::cout << min_pair.first << ' ';
        int num = min_pair.second;
        if (position[num] < data[num].size()) {
            heap.Modify(data[num][position[num]++]);
        } else {
            heap.Modify(INT_MAX);
        }
    }

    return 0;
}
