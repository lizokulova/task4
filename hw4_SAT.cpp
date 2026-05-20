#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Edge {
    int u, v;
};
struct Graph {
    int V, E;
    vector<Edge> edges;
};

struct SATSolver {
    vector<vector<int>> clauses;
    void addClause(vector<int> clause) {clauses.push_back(clause);}
    void clear() {clauses.clear();}
    bool clauseIsFalse(const vector<int>& clause, const vector<int>& value) {
        for (int lit:clause){
            int x = abs(lit);
            if (value[x] == -1) return false;
            if (lit > 0 && value[x] == 1) return false;
            if (lit < 0 && value[x] == 0) return false;
        }
        return true;
    }

    bool allClausesTrue(const vector<int>& value) {
        for (const vector<int>& clause : clauses) {
            bool ok = false;

            for (int lit : clause) {
                int x = abs(lit);
                if (lit > 0 && value[x] == 1)ok = true;
                if (lit < 0 && value[x] == 0) ok = true;
            }
            if (!ok) return false;
        }

        return true;
    }

    bool hasBadClause(const vector<int>& value) {
        for (const vector<int>& clause : clauses) {
            if (clauseIsFalse(clause, value)) {
                return true;
            }
        }

        return false;
    }

    bool solve(int pos, int variablesCount, vector<int>& value) {
        if (hasBadClause(value)) return false;
        if (pos > variablesCount) return allClausesTrue(value);
        value[pos] = 1;
        if (solve(pos + 1, variablesCount, value))return true;
        value[pos] = 0;
        if (solve(pos + 1, variablesCount, value)) return true;
        value[pos] = -1;

        return false;
    }
};

struct ColoringBuilder {
    int K;
    Graph G;

    int varId(int v, int color) {return v*K+color+1;}

    void buildSAT(SATSolver& solver){
        solver.clear();
        for (int v = 0; v < G.V; v++) {
            vector<int> clause;
            for (int c = 0; c < K; c++) clause.push_back(varId(v, c));
            solver.addClause(clause);
        }
        for (int v = 0; v < G.V; v++){
            for (int c1 = 0; c1 < K; c1++){
                for (int c2 = c1 + 1; c2 < K; c2++){
                    solver.addClause({-varId(v, c1),-varId(v, c2)});
                }
            }
        }
        for (int i = 0; i < G.E; i++) {
            int u = G.edges[i].u;
            int v = G.edges[i].v;
            for (int c = 0; c < K; c++){
                solver.addClause({-varId(u, c),-varId(v, c)});
            }
        }
    }

    vector<int> getColoring(const vector<int>& value) {
        vector<int> colors(G.V, -1);

        for (int v = 0; v < G.V; v++) {
            for (int c = 0; c < K; c++) {
                if (value[varId(v, c)] == 1) {
                    colors[v] = c;
                }
            }
        }
        return colors;
    }
};

Graph G;
vector<int> answer;

int main() {
    cin >> G.V >> G.E;
    G.edges.resize(G.E);
    for (int i = 0; i < G.E; i++) cin >> G.edges[i].u >> G.edges[i].v;
    SATSolver solver;
    for (int K = 1; K <= G.V; K++) {
        ColoringBuilder builder{K, G};
        builder.buildSAT(solver);
        int variablesCount = G.V * K;
        vector<int> value(variablesCount + 1, -1);
        if (solver.solve(1, variablesCount, value)) {
            answer = builder.getColoring(value);
            cout << K << endl;
            for (int i = 0; i < G.V; i++) cout << answer[i] << " ";
            cout << endl;
            return 0;
        }
    }

    return 0;
}
