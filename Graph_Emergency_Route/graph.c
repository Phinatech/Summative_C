#include "graph.h"
#include <string.h>

const char *NAMES[N] = {"S1", "S2", "S3", "S4", "S5", "S6", "X"};

int node_index(const char *name) {
    for (int i = 0; i < N; i++) {
        if (strcmp(NAMES[i], name) == 0) return i;
    }
    return -1;
}

void init_graph(int g[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            g[i][j] = (i == j) ? 0 : INF;
        }
    }
}

// Bidirectional (undirected) link
void add_edge(int g[N][N], int u, int v, int w) {
    if (u < 0 || u >= N || v < 0 || v >= N) return; // safety
    g[u][v] = w;
    g[v][u] = w;
}

void build_default_topology(int g[N][N]) {
    // The network given in the question (all bidirectional)
    add_edge(g, node_index("S1"), node_index("S2"), 8);
    add_edge(g, node_index("S1"), node_index("S4"), 20);
    add_edge(g, node_index("S2"), node_index("S3"), 7);
    add_edge(g, node_index("S3"), node_index("S6"), 12);
    add_edge(g, node_index("S4"), node_index("S5"), 4);
    add_edge(g, node_index("S5"), node_index("S6"), 6);
    add_edge(g, node_index("S2"), node_index("X"), 3);
    add_edge(g, node_index("X"), node_index("S5"), 5);
}