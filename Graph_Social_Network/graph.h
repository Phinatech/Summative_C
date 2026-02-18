#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

#define ID_MAX 16

typedef struct {
    char **ids;   // user IDs
    int  **adj;   // adjacency matrix
    int n;        // number of users
} Graph;

void graph_init(Graph *g);
void graph_free(Graph *g);

/* User operations */
bool add_user(Graph *g, const char *id, bool verbose);
bool remove_user(Graph *g, const char *id, bool verbose);

/* Interaction operations */
bool add_interaction(Graph *g, const char *from, const char *to, bool verbose);
bool remove_interaction(Graph *g, const char *from, const char *to, bool verbose);

/* Queries & output */
void query_user(const Graph *g, const char *id);
void print_adjacency_matrix(const Graph *g);

/* Seed initial dataset from question */
void seed_initial_graph(Graph *g);

#endif
