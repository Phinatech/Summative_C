#include <stdio.h>
#include "graph.h"
#include "dijkstra.h"
#include "utils.h"

int main(void) {
    int g[N][N];
    init_graph(g);
    build_default_topology(g);

    char start[32], dest[32];

    printf("Enter start node (e.g., S1): ");
    if (!read_token(start, sizeof(start))) {
        printf("Invalid input.\n");
        print_valid_nodes();
        return 0;
    }

    printf("Enter destination node (e.g., S6): ");
    if (!read_token(dest, sizeof(dest))) {
        printf("Invalid input.\n");
        print_valid_nodes();
        return 0;
    }

    int s = node_index(start);
    int t = node_index(dest);

    if (s == -1 || t == -1) {
        printf("Unknown server/switch name.\n");
        print_valid_nodes();
        return 0;
    }

    int dist[N], prev[N];
    dijkstra(g, s, dist, prev);

    if (dist[t] >= INF) {
        printf("No route found from %s to %s.\n", NAMES[s], NAMES[t]);
        return 0;
    }

    int path[N];
    int path_len = build_path(prev, s, t, path);

    if (path_len == 0) {
        printf("No route found from %s to %s.\n", NAMES[s], NAMES[t]);
        return 0;
    }

    printf("\nOptimal routing path: ");
    for (int i = 0; i < path_len; i++) {
        printf("%s", NAMES[path[i]]);
        if (i != path_len - 1) printf(" -> ");
    }
    printf("\n");

    printf("Total latency: %d\n", dist[t]);

    return 0;
}
