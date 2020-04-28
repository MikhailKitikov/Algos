#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

struct HeapElement {
    enum HeapPos { LEFT, RIGHT, NONE };

    int value;    
    HeapPos what_heap;
    size_t pos;
};

class Heap {
public:
    enum HEAP_TYPE { MIN, MAX };

    explicit Heap(HEAP_TYPE type) : type_(type) {}

    void HeapifyDown(size_t start = 0) {
        if (Size() == 0) {
            return;
        }
        size_t pos = start;
        while (pos < heap.size()) {
            size_t min_child = 2 * pos + 1;
            if (min_child >= heap.size()) {
                return;
            }
            if (2 * pos + 2 < heap.size() && Comp(heap[2 * pos + 2], heap[2 * pos + 1])) {
                min_child = 2 * pos + 2;
            }
            if (Comp(heap[min_child], heap[pos])) {
                std::swap(heap[pos]->pos, heap[min_child]->pos);
                std::swap(heap[pos], heap[min_child]);
                pos = min_child;
                continue;
            }
            return;
        }
    }

    void HeapifyUp(size_t start = SIZE_MAX) {
        if (Size() == 0) {
            return;
        }
        size_t pos = std::min(start, heap.size() - 1);
        while (pos >= 1) {
            size_t parent = (pos - 1) / 2;
            if (Comp(heap[pos], heap[parent])) {
                std::swap(heap[pos]->pos, heap[parent]->pos);
                std::swap(heap[pos], heap[parent]);
                pos = parent;
                continue;
            }
            return;
        }
    }

    void Add(HeapElement* item) {
        heap.push_back(item);
        item->pos = heap.size() - 1;
        HeapifyUp();
    }

    void Remove(size_t pos) {
        if (pos == heap.size() - 1) {
            heap.pop_back();
            return;
        }
        std::swap(heap[pos]->pos, heap[heap.size() - 1]->pos);
        std::swap(heap[pos], heap[heap.size() - 1]);
        heap[heap.size() - 1]->what_heap = HeapElement::HeapPos::NONE;
        heap.pop_back();
        HeapifyDown(pos);
        HeapifyUp(pos);
    }

    HeapElement* GetExtr() {
        if (Size() == 0) {
            return nullptr;
        }
        return heap[0];
    }

    size_t Size() {
        return heap.size();
    }

private:
    std::vector<HeapElement*> heap;
    HEAP_TYPE type_;

    bool Comp(HeapElement* a_arg, HeapElement* b_arg) {
        if (type_ == MIN) {
            return (a_arg->value < b_arg->value);
        }
        return (a_arg->value > b_arg->value);
    }
};

int main() {
    
    size_t n_size, m_size, k_stat;
    std::cin >> n_size >> m_size >> k_stat;
    std::vector<int> numbers(n_size);
    for (size_t i = 0; i < n_size; ++i) {
        std::cin >> numbers[i];
    }

    if (m_size == 0) {
        return 0;
    }

    std::string shifts;
    std::cin >> shifts;

    Heap l_heap(Heap::HEAP_TYPE::MAX);
    Heap r_heap(Heap::HEAP_TYPE::MIN);

    std::vector<HeapElement> elements(n_size);
    for (size_t i = 0; i < n_size; ++i) {
        elements[i] = { numbers[i], HeapElement::HeapPos::NONE, 0 };
    }
    elements[0].what_heap = HeapElement::HeapPos::LEFT;
    l_heap.Add(&elements[0]);

    size_t left_ptr = 0, right_ptr = 0;
    for (size_t shift = 0; shift < m_size; ++shift) {
        if (shifts[shift] == 'L') {
            ++left_ptr;
            if (elements[left_ptr - 1].what_heap == HeapElement::HeapPos::LEFT) {
                l_heap.Remove(elements[left_ptr - 1].pos);
                if (r_heap.Size() > 0) {
                    auto top = r_heap.GetExtr();
                    r_heap.Remove(top->pos);
                    top->what_heap = HeapElement::HeapPos::LEFT;
                    l_heap.Add(top);
                }
            } else {
                r_heap.Remove(elements[left_ptr - 1].pos);
            }
        } else {
            ++right_ptr;
            if (l_heap.Size() < k_stat) {
                elements[right_ptr].what_heap = HeapElement::HeapPos::LEFT;
                l_heap.Add(&elements[right_ptr]);
            } else if (numbers[right_ptr] < l_heap.GetExtr()->value) {
                auto top = l_heap.GetExtr();
                l_heap.Remove(top->pos);
                top->what_heap = HeapElement::HeapPos::RIGHT;
                r_heap.Add(top);
                l_heap.Add(&elements[right_ptr]);
                elements[right_ptr].what_heap = HeapElement::HeapPos::LEFT;
            } else {
                r_heap.Add(&elements[right_ptr]);
                elements[right_ptr].what_heap = HeapElement::HeapPos::RIGHT;
            }
        }
        if (l_heap.Size() < k_stat) {
            std::cout << -1 << '\n';
        } else {
            auto res = l_heap.GetExtr();
            std::cout << res->value << '\n';
        }
    }

    return 0;
}
