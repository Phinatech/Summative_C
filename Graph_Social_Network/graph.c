#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- Internal Helpers ---------- */

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static char *str_dup(const char *s) {
    size_t len = strlen(s) + 1;
    char *p = (char *)malloc(len);
    if (!p) die("malloc");
    memcpy(p, s, len);
    return p;
}

static int find_user_index(const Graph *g, const char *id) {
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->ids[i], id) == 0) return i;
    }
    return -1;
}

static void expand_matrix(Graph *g) {
    int newN = g->n + 1;

    // resize ids
    char **newIds = (char **)realloc(g->ids, newN * sizeof(char *));
    if (!newIds) die("realloc ids");
    g->ids = newIds;

    // resize row pointers
    int **newAdj = (int **)realloc(g->adj, newN * sizeof(int *));
    if (!newAdj) die("realloc adj rows");
    g->adj = newAdj;

    // allocate new row
    g->adj[newN - 1] = (int *)calloc(newN, sizeof(int));
    if (!g->adj[newN - 1]) die("calloc new row");

    // expand existing rows
    for (int i = 0; i < newN - 1; i++) {
        int *newRow = (int *)realloc(g->adj[i], newN * sizeof(int));
        if (!newRow) die("realloc row expand");
        g->adj[i] = newRow;
        g->adj[i][newN - 1] = 0;
    }

    g->n = newN;
}

static void remove_matrix_index(Graph *g, int idx) {
    int oldN = g->n;
    int newN = oldN - 1;

    // free id
    free(g->ids[idx]);

    // shift ids left
    for (int i = idx; i < newN; i++) g->ids[i] = g->ids[i + 1];

    // remove row
    free(g->adj[idx]);
    for (int i = idx; i < newN; i++) g->adj[i] = g->adj[i + 1];

    // shift columns and shrink each row
    for (int r = 0; r < newN; r++) {
        for (int c = idx; c < newN; c++) {
            g->adj[r][c] = g->adj[r][c + 1];
        }
        int *shrunk = (int *)realloc(g->adj[r], newN * sizeof(int));
        if (!shrunk && newN > 0) die("realloc row shrink");
        g->adj[r] = shrunk;
    }

    // shrink ids and adj pointers
    if (newN == 0) {
        free(g->ids); g->ids = NULL;
        free(g->adj); g->adj = NULL;
    } else {
        char **shrunkIds = (char **)realloc(g->ids, newN * sizeof(char *));
        if (!shrunkIds) die("realloc ids shrink");
        g->ids = shrunkIds;

        int **shrunkAdj = (int **)realloc(g->adj, newN * sizeof(int *));
        if (!shrunkAdj) die("realloc adj rows shrink");
        g->adj = shrunkAdj;
    }

    g->n = newN;
}

/* ---------- Public API ---------- */

void graph_init(Graph *g) {
    g->ids = NULL;
    g->adj = NULL;
    g->n = 0;
}

void graph_free(Graph *g) {
    if (g->ids) {
        for (int i = 0; i < g->n; i++) free(g->ids[i]);
        free(g->ids);
    }
    if (g->adj) {
        for (int i = 0; i < g->n; i++) free(g->adj[i]);
        free(g->adj);
    }
    g->ids = NULL;
    g->adj = NULL;
    g->n = 0;
}

bool add_user(Graph *g, const char *id, bool verbose) {
    if (find_user_index(g, id) != -1) {
        if (verbose) printf("⚠️  User '%s' already exists.\n", id);
        return false;
    }
    expand_matrix(g);
    g->ids[g->n - 1] = str_dup(id);
    if (verbose) printf("✅ Added user '%s'.\n", id);
    return true;
}

bool remove_user(Graph *g, const char *id, bool verbose) {
    int idx = find_user_index(g, id);
    if (idx == -1) {
        if (verbose) printf("⚠️  User '%s' not found. Nothing removed.\n", id);
        return false;
    }
    remove_matrix_index(g, idx);
    if (verbose) printf("✅ Removed user '%s' (and all related interactions).\n", id);
    return true;
}

bool add_interaction(Graph *g, const char *from, const char *to, bool verbose) {
    int i = find_user_index(g, from);
    int j = find_user_index(g, to);

    if (i == -1 || j == -1) {
        if (verbose) {
            printf("❌ Cannot add interaction. Unknown user(s): ");
            if (i == -1) printf("'%s' ", from);
            if (j == -1) printf("'%s' ", to);
            printf("\n");
        }
        return false;
    }

    if (i == j) {
        if (verbose) printf("⚠️  Self-interaction '%s' -> '%s' is not allowed.\n", from, to);
        return false;
    }

    if (g->adj[i][j] == 1) {
        if (verbose) printf("⚠️  Interaction already exists: %s -> %s\n", from, to);
        return false;
    }

    g->adj[i][j] = 1;
    if (verbose) printf("✅ Added interaction: %s -> %s\n", from, to);
    return true;
}

bool remove_interaction(Graph *g, const char *from, const char *to, bool verbose) {
    int i = find_user_index(g, from);
    int j = find_user_index(g, to);

    if (i == -1 || j == -1) {
        if (verbose) printf("❌ Cannot remove interaction: unknown user(s).\n");
        return false;
    }
    if (g->adj[i][j] == 0) {
        if (verbose) printf("⚠️  No interaction exists: %s -> %s\n", from, to);
        return false;
    }

    g->adj[i][j] = 0;
    if (verbose) printf("✅ Removed interaction: %s -> %s\n", from, to);
    return true;
}

void query_user(const Graph *g, const char *id) {
    int idx = find_user_index(g, id);
    if (idx == -1) {
        printf("❌ User '%s' not found.\n", id);
        return;
    }

    printf("\n==============================\n");
    printf(" Query Result for User: %s\n", id);
    printf("==============================\n");

    // Outgoing
    printf("Outgoing (interacts with):\n");
    int outCount = 0;
    for (int j = 0; j < g->n; j++) {
        if (g->adj[idx][j]) {
            printf("  -> %s\n", g->ids[j]);
            outCount++;
        }
    }
    if (outCount == 0) printf("  (none)\n");

    // Incoming
    printf("Incoming (interacted by):\n");
    int inCount = 0;
    for (int i = 0; i < g->n; i++) {
        if (g->adj[i][idx]) {
            printf("  <- %s\n", g->ids[i]);
            inCount++;
        }
    }
    if (inCount == 0) printf("  (none)\n");

    printf("\nSummary: %d outgoing, %d incoming\n", outCount, inCount);
    printf("==============================\n\n");
}

void print_adjacency_matrix(const Graph *g) {
    if (g->n == 0) {
        printf("\nAdjacency Matrix: (empty graph)\n\n");
        return;
    }

    printf("\n====================================\n");
    printf(" Adjacency Matrix (Directed Graph)\n");
    printf(" 1 = interaction exists, 0 = none\n");
    printf("====================================\n\n");

    // column header
    printf("%8s %10s", "Index", "From\\To");
    for (int j = 0; j < g->n; j++) {
        printf("%8d", j);
    }
    printf("\n");

    printf("%8s %10s", "", "");
    for (int j = 0; j < g->n; j++) {
        printf("%8s", g->ids[j]);
    }
    printf("\n\n");

    // rows
    for (int i = 0; i < g->n; i++) {
        printf("%8d %10s", i, g->ids[i]);
        for (int j = 0; j < g->n; j++) {
            printf("%8d", g->adj[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void seed_initial_graph(Graph *g) {
    // silent seeding (no noisy prints)
    add_user(g, "U101", false);
    add_user(g, "U102", false);
    add_user(g, "U103", false);
    add_user(g, "U104", false);
    add_user(g, "U105", false);
    add_user(g, "U106", false);
    add_user(g, "U107", false);
    add_user(g, "U108", false);

    add_interaction(g, "U101", "U102", false);
    add_interaction(g, "U101", "U103", false);
    add_interaction(g, "U102", "U104", false);
    add_interaction(g, "U103", "U105", false);
    add_interaction(g, "U104", "U105", false);
    add_interaction(g, "U104", "U106", false);
    add_interaction(g, "U105", "U107", false);
    add_interaction(g, "U106", "U108", false);
    