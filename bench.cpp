#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef vector<int> vi;
#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define sz(x) (int)(x).size()

#include "Dinic.h"
#include "PushRelabel.h"
#include "PushRelabelBase.hpp"

vector<tuple<int,int,int>> generateGraph(int n, double density, int maxCap, unsigned seed) {
    vector<tuple<int,int,int>> edges;
    mt19937 rng(seed);
    uniform_int_distribution<int> capDist(1, maxCap);
    
    int totalPossible = n * (n-1);
    int targetEdges = (int)(density * totalPossible);
    
    set<pair<int,int>> used;
    
    for (int i = 0; i < n-1; i++) {
        edges.push_back({i, i+1, capDist(rng)});
        used.insert({i, i+1});
    }
    
    while ((int)edges.size() < targetEdges) {
        int u = rng() % n;
        int v = rng() % n;
        if (u != v && used.find({u,v}) == used.end()) {
            edges.push_back({u, v, capDist(rng)});
            used.insert({u,v});
        }
    }
    
    return edges;
}

struct DinicAdapter {
    Dinic* d;
    DinicAdapter(int n) : d(new Dinic(n)) {}
    ~DinicAdapter() { delete d; }
    void addEdge(int u, int v, int c) { d->addEdge(u, v, c); }
    ll maxFlow(int s, int t) { return d->calc(s, t); }
};

struct HLPPAdapter {
    PushRelabel* pr;
    HLPPAdapter(int n) : pr(new PushRelabel(n)) {}
    ~HLPPAdapter() { delete pr; }
    void addEdge(int u, int v, int c) { pr->addEdge(u, v, c); }
    ll maxFlow(int s, int t) { return pr->calc(s, t); }
};

double measureDinic(int n, double density, int s, int t, const vector<unsigned>& seeds) {
    vector<double> times;
    for (unsigned seed : seeds) {
        auto edges = generateGraph(n, density, 100, seed);
        DinicAdapter dinic(n);
        for (auto [u,v,c] : edges) dinic.addEdge(u, v, c);
        auto start = chrono::high_resolution_clock::now();
        dinic.maxFlow(s, t);
        auto end = chrono::high_resolution_clock::now();
        times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

double measureHLPP(int n, double density, int s, int t, const vector<unsigned>& seeds) {
    vector<double> times;
    for (unsigned seed : seeds) {
        auto edges = generateGraph(n, density, 100, seed);
        HLPPAdapter hlpp(n);
        for (auto [u,v,c] : edges) hlpp.addEdge(u, v, c);
        auto start = chrono::high_resolution_clock::now();
        hlpp.maxFlow(s, t);
        auto end = chrono::high_resolution_clock::now();
        times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

double measureBasePR(int n, double density, int s, int t, const vector<unsigned>& seeds) {
    vector<double> times;
    for (unsigned seed : seeds) {
        auto edges = generateGraph(n, density, 100, seed);
        PushRelabelBase prBase(n, s, t);
        for (auto [u,v,c] : edges) prBase.addEdge(u, v, c);
        auto start = chrono::high_resolution_clock::now();
        prBase.maxFlow();
        auto end = chrono::high_resolution_clock::now();
        times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

void exampleGraph() {
    cout << "=== Проверка: маленький граф (поток должен быть 10) ===\n";
    int n = 4, s = 0, t = 3;
    
    DinicAdapter dinic(n);
    dinic.addEdge(0, 1, 10); dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 5); dinic.addEdge(2, 3, 5);
    dinic.addEdge(1, 2, 8); dinic.addEdge(2, 1, 8);
    
    HLPPAdapter hlpp(n);
    hlpp.addEdge(0, 1, 10); hlpp.addEdge(0, 2, 10);
    hlpp.addEdge(1, 3, 5); hlpp.addEdge(2, 3, 5);
    hlpp.addEdge(1, 2, 8); hlpp.addEdge(2, 1, 8);
    
    PushRelabelBase prBase(n, s, t);
    prBase.addEdge(0, 1, 10); prBase.addEdge(0, 2, 10);
    prBase.addEdge(1, 3, 5); prBase.addEdge(2, 3, 5);
    prBase.addEdge(1, 2, 8); prBase.addEdge(2, 1, 8);
    
    cout << "Dinic: " << dinic.maxFlow(s, t) << "\n";
    cout << "HLPP:  " << hlpp.maxFlow(s, t) << "\n";
    cout << "Base:  " << prBase.maxFlow() << "\n\n";
}

int main() {
    exampleGraph();
    
    vector<unsigned> seeds = {42, 123, 456, 789, 101112};
    vector<int> sizes = {100, 200, 300, 500};
    vector<double> densities = {0.3, 0.5, 0.7, 0.9};
    
    ofstream csv("benchmark_results.csv");
    csv << "n,density,algorithm,mean_ms\n";
    
    cout << "=== Бенчмарк: Dinic vs BasePR vs HLPP ===\n";
    cout << "5 разных графов, медиана\n\n";
    cout << "n\tDensity\t\tDinic(ms)\tBasePR(ms)\tHLPP(ms)\tFaster\n";
    cout << string(85, '-') << "\n";
    
    for (int n : sizes) {
        for (double d : densities) {
            int s = 0, t = n-1;
            
            double tD = measureDinic(n, d, s, t, seeds);
            double tB = measureBasePR(n, d, s, t, seeds);
            double tH = measureHLPP(n, d, s, t, seeds);
            
            csv << n << "," << d << ",Dinic," << tD << "\n";
            csv << n << "," << d << ",BasePR," << tB << "\n";
            csv << n << "," << d << ",HLPP," << tH << "\n";
            
            string faster = (tH < tD) ? "HLPP" : "Dinic";
            if (tB < tD && tB < tH) faster = "BasePR";
            
            printf("%d\t%.1f\t\t%.2f\t\t%.2f\t\t%.2f\t\t%s\n", 
                   n, d, tD, tB, tH, faster.c_str());
            cout.flush();
        }
        cout << "\n";
    }
    
    csv.close();
    cout << "\nРезультаты сохранены в benchmark_results.csv\n";
    return 0;
}