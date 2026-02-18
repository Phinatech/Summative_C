#include "dijkstra.h"

void dijkstra(int g[N][N], int src, int dist[N], int prev[N]) {
    int visited[N];

    for (int i = 0; i < N; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = 0;
    }
    dist[src] = 0;

    for (int step = 0; step < N; step++) {
        int u = -1;
        int best = INF;

        // pick unvisited node with smallest dist
        for (int i = 0; i < N; i++) {
            if (!visited[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }

        if (u == -1) break; // no more reachable nodes
        visited[u] = 1;

        // relax neighbors
        for (int v = 0; v < N; v++) {
            if (!visited[v] && g[u][v] < INF) {
                int nd = dist[u] + g[u][v];
                if (nd < dist[v]) {
                    dist[v] = nd;
                    prev[v] = u;
                }
            }
        }
    }
}

// Builds the path from src to dest into out_path (src..dest order)
// Returns length, or 0 if no path exists.
int build_path(int prev[N], int src, int dest, int out_path[N]) {
    int temp[N];
    int len = 0;

    for (int cur = dest; cur != -1; cur = prev[cur]) {
        temp[len++] = cur;
        if (cur == src) break;
    }

    if (len == 0 || temp[len - 1] != src) return 0;

    // reverse into out_path
    for (int i = 0; i < len; i++) {
        out_path[i] = temp[len - 1 - i];
    }
    return len;
}