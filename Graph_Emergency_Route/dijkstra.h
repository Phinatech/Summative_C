#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

void dijkstra(int g[N][N], int src, int dist[N], int prev[N]);
int build_path(int prev[N], int src, int dest, int out_path[N]); // returns length

#endif
