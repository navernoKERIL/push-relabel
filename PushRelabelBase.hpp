#pragma once

class PushRelabelBase {
    int n;
    vector<vector<int>> cap;
    vector<int> excess;
    vector<int> height;
    vector<vector<int>> adj;
    int s, t;

    void push(int u, int v) {
        int flow = min(excess[u], cap[u][v]);
        cap[u][v] -= flow;
        cap[v][u] += flow;
        excess[u] -= flow;
        excess[v] += flow;
    }

    void relabel(int u) {
        int min_h = INT_MAX;
        for (int v : adj[u]) {
            if (cap[u][v] > 0) {
                min_h = min(min_h, height[v]);
            }
        }
        if (min_h != INT_MAX) {
            height[u] = min_h + 1;
        }
    }

public:
    PushRelabelBase(int _n, int _s, int _t) : n(_n), s(_s), t(_t) {
        cap.assign(n, vector<int>(n, 0));
        adj.resize(n);
        excess.assign(n, 0);
        height.assign(n, 0);
    }

    void addEdge(int u, int v, int c) {
        cap[u][v] += c;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int maxFlow() {
        // Инициализация
        height[s] = n;
        for (int v : adj[s]) {
            if (cap[s][v] > 0) {
                int flow = cap[s][v];
                cap[s][v] -= flow;
                cap[v][s] += flow;
                excess[v] += flow;
                excess[s] -= flow;
            }
        }

        // Основной цикл
        while (true) {
            int u = -1;
            for (int i = 0; i < n; i++) {
                if (i != s && i != t && excess[i] > 0) {
                    u = i;
                    break;
                }
            }
            if (u == -1) break;

            bool pushed = false;
            for (int v : adj[u]) {
                if (cap[u][v] > 0 && height[u] == height[v] + 1) {
                    push(u, v);
                    pushed = true;
                    break;
                }
            }
            if (!pushed) {
                relabel(u);
            }
        }

        return excess[t];
    }
};
