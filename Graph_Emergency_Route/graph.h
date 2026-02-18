#ifndef GRAPH_H
#define GRAPH_H

#define N 7
#define INF 1000000000

extern const char *NAMES[N];

int node_index(const char *name);
void init_graph(int g[N][N]);
void add_edge(int g[N][N], int u, int v, int w);
void build_default_topology(int g[N][N]); // builds the network in the prompt

#endif
