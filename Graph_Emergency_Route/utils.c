#include "utils.h"
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void trim_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

int read_token(char *buf, size_t bufsize) {
    if (!fgets(buf, (int)bufsize, stdin)) return 0;
    trim_newline(buf);

    // skip leading spaces
    size_t i = 0;
    while (buf[i] && isspace((unsigned char)buf[i])) i++;

    if (buf[i] == '\0') return 0;

    // keep only first word
    size_t j = 0;
    while (buf[i] && !isspace((unsigned char)buf[i]) && j + 1 < bufsize) {
        buf[j++] = buf[i++];
    }
    buf[j] = '\0';
    return 1;
}

void print_valid_nodes(void) {
    printf("Valid nodes are: ");
    for (int i = 0; i < N; i++) {
        printf("%s%s", NAMES[i], (i == N - 1) ? "" : ", ");
    }
    printf("\n");
}