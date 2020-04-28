#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <climits>
#include <utility>


class GraphOracle {

private:
    int vertex_count;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<std::vector<std::pair<int, int>>> graph_rev;
    std::vector<int> curr_landmarks;
    std::vector<std::vector<int>> dist_landmarks;
    std::vector<std::vector<int>> dist_landmarks_rev;

    class AStarStruct {

    public:
        int val;
        int initVal;
        int vertex;

        AStarStruct(int val, int initVal, int pos) : val(val), initVal(initVal), 
            vertex(pos) {
        }

        bool operator < (const AStarStruct& other) const {
            return val < other.val;
        }

        bool operator <= (const AStarStruct& other) const {
            return val <= other.val;
        }
    };

    class MinHeap {

    private:
        std::vector<AStarStruct> arr;

    public:
        MinHeap() {
        }

        bool Empty() {
            return arr.size() == 0;
        }

        void HeapifyUp() {
            int pos = arr.size() - 1;
            while (pos > 0) {
                int parent = (pos - 1) >> 1;
                if (arr[pos] < arr[parent]) {
                    std::swap(arr[pos], arr[parent]);
                    pos = parent;
                } else {
                    break;
                }
            }
        }

        void HeapifyDown() {
            int pos = 0;
            while (2 * pos + 1 < static_cast<int>(arr.size())) {
                int minChild = 2 * pos + 1;
                if ((2 * pos + 2 < static_cast<int>(arr.size())) && 
                    (arr[2 * pos + 2] < arr[2 * pos + 1])) {
                    minChild = 2 * pos + 2;
                }
                if (arr[pos] <= arr[minChild]) {
                    break;
                }
                std::swap(arr[pos], arr[minChild]);
            }
        }

        void Add(AStarStruct el) {
            arr.push_back(el);
            this->HeapifyUp();
        }

        AStarStruct ExtractMin() {
            auto ans = this->arr[0];
            std::swap(this->arr[0], this->arr[this->arr.size() - 1]);
            this->arr.pop_back();
            this->HeapifyDown();
            return ans;
        }
    };

    void Dijkstra(int start, std::vector<int>& dist,
        std::vector<std::vector<std::pair<int, int>>>& g_graph) {
        dist[start] = 0;
        MinHeap heap;
        heap.Add(AStarStruct(0, 0, start));
        while (!heap.Empty()) {
            auto pair = heap.ExtractMin();
            int curr_vert = pair.vertex;
            int curr_dist = pair.val;
            if (dist[curr_vert] < curr_dist) {
                continue;
            }
            for (int index = 0; index < static_cast<int>(g_graph[curr_vert].size()); 
                ++index) {
                int to = g_graph[curr_vert][index].second;
                int arc_len = g_graph[curr_vert][index].first;
                if (dist[curr_vert] + arc_len < dist[to]) {
                    dist[to] = dist[curr_vert] + arc_len;
                    heap.Add(AStarStruct(dist[to], 0, to));
                }
            }
        }
    }

    int heuristic(int vert, int src, int dest) {
        int record = 0;
        for (int index = 0; index < 20; ++index) {
            int land = curr_landmarks[index];
            int startPotential = dist_landmarks[land][dest]
                - dist_landmarks[land][src];
            int startPotentialRev = dist_landmarks_rev[land][src]
                - dist_landmarks_rev[land][dest];

            int vertPotential = dist_landmarks[land][dest]
                - dist_landmarks[land][vert];
            int vertPotentialRev = dist_landmarks_rev[land][vert]
                - dist_landmarks_rev[land][dest];

            if (startPotential == INT_MAX || vertPotential == INT_MAX) {
                startPotential = vertPotential = 0;
            }

            if (startPotentialRev == INT_MAX || vertPotentialRev == INT_MAX) {
                startPotentialRev = vertPotentialRev = 0;
            }

            int temp_rec = std::max(vertPotential, vertPotentialRev);
            record = std::max(record, temp_rec);
        }
        return record;
    }

    int A_star(int src, int dest) {
        std::vector<int> dist(vertex_count, INT_MAX);
        dist[src] = 0;
        MinHeap heap;
        heap.Add(AStarStruct(0, 0, src));
        while (!heap.Empty()) {
            AStarStruct curr_vert = heap.ExtractMin();
            if (curr_vert.vertex == dest) {
                return dist[dest];
            }
            if (dist[curr_vert.vertex] < curr_vert.initVal) {
                continue;
            }
            for (int index = 0; index < static_cast<int>(graph[curr_vert.vertex].size()); 
                ++index) {
                int to = graph[curr_vert.vertex][index].second;
                int arc_len = graph[curr_vert.vertex][index].first;
                if (dist[curr_vert.vertex] + arc_len < dist[to]) {
                    dist[to] = dist[curr_vert.vertex] + arc_len;
                    int heuristic_dist = dist[to] + heuristic(to, src, dest);
                    heap.Add(AStarStruct(heuristic_dist, dist[to], to));
                }
            }
        }
        return -1;
    }

public:
    explicit GraphOracle(int vertex_count)
        : vertex_count(vertex_count) {
        graph.resize(vertex_count);
        graph_rev.resize(vertex_count);
    }

    void add_edge(int vertex_u, int vertex_v, int cost) {
        graph[vertex_u].emplace_back(cost, vertex_v);
        graph_rev[vertex_v].emplace_back(cost, vertex_u);
    }

    void initialize() {
        // pick landmarks & calculate landmark distances
        std::mt19937 mt_rand(time(0));
        dist_landmarks.resize(96);
        for (int index = 0; index < 96; ++index) {
            dist_landmarks[index].resize(vertex_count, INT_MAX);
            int choice = mt_rand() % vertex_count;
            Dijkstra(choice, dist_landmarks[index], graph);
        }
        dist_landmarks_rev.resize(96);
        for (int index = 0; index < 96; ++index) {
            dist_landmarks_rev[index].resize(vertex_count, INT_MAX);
            int choice = mt_rand() % vertex_count;
            Dijkstra(choice, dist_landmarks_rev[index], graph_rev);
        }
    }

    int shortest_path(int vertex_u, int vertex_v) {
        curr_landmarks.clear();
        MinHeap landmark_heap;

        for (int landmark_ind = 0; landmark_ind < 96; ++landmark_ind) {
            if (dist_landmarks[landmark_ind][vertex_v] == INT_MAX && 
                dist_landmarks[landmark_ind][vertex_u] != INT_MAX) {
                return -1;
            }
            int val = std::max(dist_landmarks[landmark_ind][vertex_v] - 
                dist_landmarks[landmark_ind][vertex_u],
                dist_landmarks_rev[landmark_ind][vertex_u] - 
                dist_landmarks_rev[landmark_ind][vertex_v]);

            landmark_heap.Add(AStarStruct(-val, 0, landmark_ind));
        }

        for (int pick = 0; pick < 20; ++pick) {
            curr_landmarks.push_back(landmark_heap.ExtractMin().vertex);
        }

        return A_star(vertex_u, vertex_v);
    }
};
