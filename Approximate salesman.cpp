#include <fstream> 
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <iomanip>
#include <iostream>
#include <stack>
//#include <time.h>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

typedef long long ll;
typedef vector<ll> vll;
typedef vector<int> vi;
typedef vector<vi> vvi;

const ll INF = LLONG_MAX;

int n;
vll x, y;
vvi mst;
vi shortcut;
ll mst_2_optimal_tsp;
vi index;

ll dist(int a, int b) {
	return (abs(x[a] - x[b]) + abs(y[a] - y[b]));
}

struct Edge {
	ll weight;
	int to;
	Edge() : weight(INF), to(-1) {}
	Edge(ll w, int t) : weight(w), to(t) {}
};

void prim()
{
	int free_cnt = n;
	vector<int> free(n);
	for (int i = 0; i < n; ++i) free[i] = i;
	vector<Edge> min_edge(n); min_edge[0].weight = 0ll;

	for (int i = 0; i < n; ++i) {
		int v = -1;
		for (int j = 0; j < free_cnt; ++j) {
			if (v == -1 || min_edge[free[j]].weight < min_edge[v].weight)
				v = j;
		}
		int vert = free[v];
		swap(free[v], free[free_cnt - 1]);
		--free_cnt;
		if (min_edge[vert].to != -1) {
			mst[vert][++index[vert]] = min_edge[vert].to;
			mst[min_edge[vert].to][++index[min_edge[vert].to]] = vert;
		}
		for (int to = 0; to < n; ++to) {
			if (dist(vert, to) < min_edge[to].weight) {
				min_edge[to].weight = dist(vert, to);
				min_edge[to].to = vert;
			}				
		}
	}
}

void traverse_mst()
{
	stack<int> q; q.push(0);
	vector<char> vis(n, 0); vis[0] = 1;
	int ans_ind = 0;
	while (!q.empty()) {
		int v = q.top();
		shortcut[ans_ind++] = v;
		q.pop();
		for (int i = 0; i <= index[v]; ++i) {
			if (vis[mst[v][i]]) continue;
			q.push(mst[v][i]);
			vis[mst[v][i]] = 1;
		}
	}
	shortcut[ans_ind] = 0;

	mst_2_optimal_tsp = 0;
	for (int i = 0; i < n; ++i) {
		mst_2_optimal_tsp += dist(shortcut[i], shortcut[i + 1]);
	}
}

int main()
{
	//clock_t tStart = clock();
	fin >> n;
	mst.resize(n, vi(n));
	x.resize(n);
	y.resize(n);
	index.resize(n, -1);
	shortcut.resize(n + 1);
	for (int i = 0; i < n; ++i)
		fin >> x[i] >> y[i];

	prim();
	traverse_mst();

	for (auto x : shortcut) {
		fout << x + 1 << ' ';
	}
	fout << endl << mst_2_optimal_tsp;
	//cout << "Time taken: " << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
	//system("pause");

	return 0;
}