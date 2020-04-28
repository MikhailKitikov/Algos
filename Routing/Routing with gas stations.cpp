#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <limits.h>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

#define INF LLONG_MAX
typedef long long LL;

void Dijkstra(vector<vector<LL> >& g, int n, int targV,
	vector<LL>& dist, vector<int>& parent,
	vector<LL>* distEx, vector<int>* parentEx, bool flag)
{
	priority_queue<pair<LL, int> >  q;
	q.emplace(0, targV);
	vector<int> mark(n, 0);

	while (!q.empty()) {
		int v = q.top().second;
		LL currDist = q.top().first;
		q.pop();
		if (++mark[v] > 2) continue;
		if (mark[v] == 1 && currDist > dist[v]
			|| flag && mark[v] == 2 && currDist > (*distEx)[v]) continue;
		for (int to = 0; to < n; ++to) {
			if (g[v][to] == -1) continue;
			if (mark[v] == 1) {
				if (dist[v] == INF) continue;
				if (mark[to] == 0 && dist[v] + g[v][to] < dist[to]) {
					dist[to] = dist[v] + g[v][to];
					parent[to] = v;
					q.emplace(-dist[to], to);
				}
				else if (flag && mark[to] == 1 && (dist[v] + g[v][to] < (*distEx)[to]) && to != parent[v]) {
					(*distEx)[to] = dist[v] + g[v][to];
					(*parentEx)[to] = v;
					q.emplace(-(*distEx)[to], to);
				}
			}
			else if (flag && (*distEx)[v] != INF && mark[to] == 1 && (*distEx)[v] + g[v][to] < (*distEx)[to]) {
				(*distEx)[to] = dist[v] + g[v][to];
				(*parentEx)[to] = v;
				q.emplace(-(*distEx)[to], to);
			}
		}
		if (flag && mark[v] == 1)
			q.push(make_pair(-currDist, v));
	}
}

vector<int>* doubleRoute(vector<vector<LL> >& g, int x, int z, int from, int to, 
	vector<LL>& dist, vector<int>& parent, bool flag, int prevEx)
{
	vector<int>* route = new vector<int>();
	int v = to, w = (flag ? prevEx : parent[v]);
	route->push_back(v);
	v = w;
	while (v != from) {
		route->push_back(v);
		v = parent[v];
	}
	route->push_back(from);
	int l = g[w][to];
	if (l <= dist[to] / 2) { // берем последнее		
		reverse(route->begin(), route->end());
		int k = (1.0 * (z - (dist[to] - l) * x) / (1.0 * l * x) - 3) / 2;
		route->push_back(w);
		route->push_back(to);
		while (k > 0) {
			route->push_back(w);
			route->push_back(to);
			--k;
		}
	}
	else {
		v = (*route)[route->size() - 2];
		l = g[from][v];
		int k = (1.0 * (z - (dist[to] - l) * x) / (1.0 * l * x) - 3) / 2;
		route->push_back(v);
		route->push_back(from);
		while (k > 0) {
			route->push_back(v);
			route->push_back(from);
			--k;
		}
		reverse(route->begin(), route->end());
	}	
	return route;
}

void findShortestPossibleRoute(vector<vector<LL> >& g, vector<vector<LL> >& d, vector<int>& chargeCost,
	int n, int z, int x, int targV, int a, int b, map< pair<int, int>, vector<int> >& routesD,
	vector<LL>& dist, vector<int>& parent, vector<LL>& distEx, vector<int>& parentEx)
{
	Dijkstra(g, n, targV, dist, parent, &distEx, &parentEx, true);

	for (int to = 0; to < n; ++to) {
		if (!chargeCost[to] || to == a || to == b || to == targV) continue;
		if (parent[to] == targV) {
			if (dist[to] * x > z) 
				d[targV][to] = INF;
			else if (dist[to] * x <= z / 3) {
				int l = dist[to];
				int k = (1.0 * z / (1.0 * l * x) - 3) / 2;
				vector<int>* route = new vector<int>();
				route->push_back(targV);
				route->push_back(to);
				route->push_back(targV);
				route->push_back(to);
				while (k > 0) {
					route->push_back(targV);
					route->push_back(to);
					--k;
				}
				routesD[make_pair(targV, to)] = *route;
				d[targV][to] = chargeCost[to];
			}
			else {
				if (distEx[to] * x > z) 
					d[targV][to] = INF;
				else if (distEx[to] * x > z / 2) {
					vector<int>* route = new vector<int>();
					int v = to, w = parentEx[v];
					route->push_back(v);
					v = w;
					while (v != targV) {
						route->push_back(v);
						v = parent[v];
					}
					route->push_back(targV);
					reverse(route->begin(), route->end());
					routesD[make_pair(targV, to)] = *route;
					d[targV][to] = chargeCost[to];
				}
				else {
					routesD[make_pair(targV, to)] = *doubleRoute(g, x, z, targV, to, distEx, parent, true, parentEx[to]);
					d[targV][to] = chargeCost[to];
				}
			}
		}
		else {
			if (dist[to] * x > z) 
				d[targV][to] = INF;
			else if (dist[to] * x > z / 2) {
				vector<int>* route = new vector<int>();
				int v = to;
				while (v != targV) {
					route->push_back(v);
					v = parent[v];
				}
				route->push_back(targV);
				reverse(route->begin(), route->end());
				routesD[make_pair(targV, to)] = *route;
				d[targV][to] = chargeCost[to];
			}
			else {
				routesD[make_pair(targV, to)] = *doubleRoute(g, x, z, targV, to, dist, parent, false, -1);
				d[targV][to] = chargeCost[to];
			}
		}
	}
}

vector<int>* restorePath(int a, int b, vector<int>& chargeStations, map< pair<int, int>, vector<int> >& routesD)
{
	vector<int>* route = new vector<int>();
	int i = 0;
	while (chargeStations[i] != b) {
		if (i == 0) route->push_back(chargeStations[i]);
		else route->push_back(-chargeStations[i]);
		vector<int>* tempRoute = &routesD[make_pair(chargeStations[i], chargeStations[i + 1])];
		vector<int>::iterator begit = tempRoute->begin();
		begit++;
		vector<int>::iterator endit = tempRoute->end();
		endit--;
		route->insert(route->end(), begit, endit);
		++i;
	}
	route->push_back(b);
	return route;
}

void solve(int& n, int& m, int& z, int&x, int& a, int& b, vector< vector<LL> >& g, vector<int>& chargeCost)
{
	vector<vector<LL> > d(n, vector<LL>(n, -1));
	map< pair<int, int>, vector<int> > routesD;

	// (u,b) distances
	vector<LL> distToB(n, INF); distToB[b] = 0;	
	vector<int> parentB(n, -1);
	Dijkstra(g, n, b, distToB, parentB, nullptr, nullptr, false);
	for (int i = 0; i < n; ++i) {
		if (i != a && !chargeCost[i]) continue;
		if (distToB[i] * x <= z) {
			d[i][b] = 0;
			vector<int>* route = new vector<int>();
			int v = i;
			while (v != b) {
				route->push_back(v);
				v = parentB[v];
			}
			route->push_back(b);
			routesD[make_pair(i, b)] = *route;
		}
		else
			d[i][b] = INF;
	}

	// (u,v) distances
	for (int i = 0; i < n; ++i) {
		if ((i != a && !chargeCost[i]) || i == b) continue;
		vector<LL> distFromV(n, INF); distFromV[i] = 0;		
		vector<LL> distFromVEx(n, INF);	
		vector<int> parentV(n, -1);
		vector<int> parentVEx(n, -1);
		findShortestPossibleRoute(g, d, chargeCost, n, z, x, i, a,b, routesD, distFromV, parentV, distFromVEx, parentVEx);
	}

	// finding final cheapest route in d
	vector<LL> distFromA(n, INF); 
	distFromA[a] = 0;
	vector<int> parentA(n, -1);
	Dijkstra(d, n, a, distFromA, parentA, nullptr, nullptr, false);

	if (distFromA[b] == INF) {
		fout << "No";
		return;
	}

	vector<int> chargeStations;
	int v = b;
	while (v != a) {
		chargeStations.push_back(v);
		v = parentA[v];
	}
	chargeStations.push_back(a);
	reverse(chargeStations.begin(), chargeStations.end());
	vector<int>* ans = restorePath(a, b, chargeStations, routesD);
	fout << "Yes\n";
	for (int i = 0; i < ans->size(); ++i) {
		fout << ((*ans)[i] < 0 ? (*ans)[i] - 1 : (*ans)[i] + 1) << ' '; ///
	}
}

int main()
{
	int n, m, z, x, a, b;
	fin >> n >> m >> z >> x >> a >> b;
	--a; --b;

	vector< vector<LL> > g(n, vector<LL>(n, -1));
	vector<int> chargeCost(n, 0);

	for (int i = 0; i < n; ++i) {
		fin >> chargeCost[i];
	}

	int v, w, d;
	for (int i = 0; i < m; ++i) {
		fin >> v >> w >> d;
		g[v - 1][w - 1] = g[w - 1][v - 1] = d;
	}

	solve(n, m, z, x, a, b, g, chargeCost);	

	return 0;
}