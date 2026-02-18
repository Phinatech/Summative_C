#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

int read_token(char *buf, size_t bufsize); // reads one word safely (returns 1 ok, 0 fail)
void print_valid_nodes(void);
void trim_newline(char *s);

#endif
