#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define MAGIC "HUF1"
#define ALPHABET 256

typedef struct Node {
    uint32_t freq;
    int symbol;               // 0..255 leaf, -1 internal
    struct Node *left, *right;
} Node;

/* ===================== Error helpers ===================== */
static void die_perror(const char *msg) {
    fprintf(stderr, "Error: %s (%s)\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static void *xcalloc(size_t n, size_t sz) {
    void *p = calloc(n, sz);
    if (!p) die_perror("Out of memory (calloc)");
    return p;
}

static void *xrealloc(void *p, size_t sz) {
    void *q = realloc(p, sz);
    if (!q) die_perror("Out of memory (realloc)");
    return q;
}

/* ===================== File size ===================== */
static uint64_t file_size_bytes(FILE *f) {
    if (fseek(f, 0, SEEK_END) != 0) die_perror("fseek end failed");
    long end = ftell(f);
    if (end < 0) die_perror("ftell failed");
    if (fseek(f, 0, SEEK_SET) != 0) die_perror("fseek start failed");
    return (uint64_t)end;
}

/* ===================== Huffman tree utilities ===================== */
static Node *node_new(uint32_t freq, int symbol, Node *l, Node *r) {
    Node *n = (Node*)xcalloc(1, sizeof(Node));
    n->freq = freq;
    n->symbol = symbol;
    n->left = l;
    n->right = r;
    return n;
}

static void tree_free(Node *root) {
    if (!root) return;
    tree_free(root->left);
    tree_free(root->right);
    free(root);
}

/* ===================== Min-heap (priority queue) ===================== */
typedef struct {
    Node **a;
    int size;
    int cap;
} MinHeap;

static MinHeap *heap_new(int cap) {
    MinHeap *h = (MinHeap*)xcalloc(1, sizeof(MinHeap));
    h->a = (Node**)xcalloc((size_t)cap, sizeof(Node*));
    h->cap = cap;
    return h;
}

static void heap_free(MinHeap *h) {
    if (!h) return;
    free(h->a);
    free(h);
}

static void heap_swap(Node **x, Node **y) { Node *t=*x; *x=*y; *y=t; }

static void heap_push(MinHeap *h, Node *n) {
    if (h->size >= h->cap) {
        h->cap = (h->cap == 0) ? 8 : h->cap * 2;
        h->a = (Node**)xrealloc(h->a, (size_t)h->cap * sizeof(Node*));
    }

    int i = h->size++;
    h->a[i] = n;

    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->a[p]->freq <= h->a[i]->freq) break;
        heap_swap(&h->a[p], &h->a[i]);
        i = p;
    }
}

static Node *heap_pop(MinHeap *h) {
    if (h->size == 0) return NULL;

    Node *top = h->a[0];
    h->a[0] = h->a[--h->size];

    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, s = i;
        if (l < h->size && h->a[l]->freq < h->a[s]->freq) s = l;
        if (r < h->size && h->a[r]->freq < h->a[s]->freq) s = r;
        if (s == i) break;
        heap_swap(&h->a[i], &h->a[s]);
        i = s;
    }
    return top;
}

/* ===================== Build Huffman tree ===================== */
static Node *build_huffman_tree(uint32_t freq[ALPHABET]) {
    MinHeap *h = heap_new(256);

    for (int i = 0; i < ALPHABET; i++) {
        if (freq[i] > 0) heap_push(h, node_new(freq[i], i, NULL, NULL));
    }

    if (h->size == 0) { // empty file
        heap_free(h);
        return NULL;
    }

    if (h->size == 1) { // single-symbol file: create parent
        Node *only = heap_pop(h);
        Node *parent = node_new(only->freq, -1, only, NULL);
        heap_free(h);
        return parent;
    }

    while (h->size > 1) {
        Node *a = heap_pop(h);
        Node *b = heap_pop(h);
        Node *p = node_new(a->freq + b->freq, -1, a, b);
        heap_push(h, p);
    }

    Node *root = heap_pop(h);
    heap_free(h);
    return root;
}

/* ===================== Huffman codes ===================== */
typedef struct {
    uint32_t bits[8]; // up to 256 bits
    int bitlen;
} Code;

static void code_clear(Code *c) { memset(c, 0, sizeof(*c)); }

static void code_push_bit(Code *c, int bit) {
    int idx = c->bitlen / 32;
    int off = c->bitlen % 32;
    if (bit) c->bits[idx] |= (1u << off);
    c->bitlen++;
}

static void build_codes_dfs(Node *root, Code codes[ALPHABET], const Code *path) {
    if (!root) return;

    if (root->symbol >= 0) {
        codes[root->symbol] = *path;

        // For single-symbol files, force at least 1 bit
        if (codes[root->symbol].bitlen == 0) {
            Code tmp = *path;
            code_push_bit(&tmp, 0);
            codes[root->symbol] = tmp;
        }
        return;
    }

    if (root->left) {
        Code next = *path;
        code_push_bit(&next, 0);
        build_codes_dfs(root->left, codes, &next);
    }
    if (root->right) {
        Code next = *path;
        code_push_bit(&next, 1);
        build_codes_dfs(root->right, codes, &next);
    }
}

/* ===================== Bit writer ===================== */
typedef struct {
    FILE *f;
    uint8_t buf;
    int fill; // 0..7
} BitWriter;

static void bw_init(BitWriter *bw, FILE *f) { bw->f = f; bw->buf = 0; bw->fill = 0; }

static void bw_write_bit(BitWriter *bw, int bit) {
    if (bit) bw->buf |= (1u << bw->fill);
    bw->fill++;
    if (bw->fill == 8) {
        if (fputc(bw->buf, bw->f) == EOF) die_perror("write compressed byte failed");
        bw->buf = 0;
        bw->fill = 0;
    }
}

static void bw_write_code(BitWriter *bw, const Code *c) {
    for (int i = 0; i < c->bitlen; i++) {
        int idx = i / 32, off = i % 32;
        int bit = (c->bits[idx] >> off) & 1u;
        bw_write_bit(bw, bit);
    }
}

static void bw_flush(BitWriter *bw) {
    if (bw->fill > 0) {
        if (fputc(bw->buf, bw->f) == EOF) die_perror("flush compressed byte failed");
        bw->buf = 0;
        bw->fill = 0;
    }
}

/* ===================== Header format ===================== */
static void write_header(FILE *out, uint64_t original_size, uint32_t freq[ALPHABET]) {
    if (fwrite(MAGIC, 1, 4, out) != 4) die_perror("write magic failed");
    if (fwrite(&original_size, sizeof(original_size), 1, out) != 1) die_perror("write original size failed");
    if (fwrite(freq, sizeof(uint32_t), ALPHABET, out) != ALPHABET) die_perror("write frequency table failed");
}

/* ===================== Main ===================== */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_log.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *infile = argv[1];
    const char *outfile = "compressed.log";

    FILE *in = fopen(infile, "rb");
    if (!in) die_perror("open input file failed");

    uint64_t original_size = file_size_bytes(in);

    uint32_t freq[ALPHABET] = {0};
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        freq[(uint8_t)ch]++;
    }
    if (ferror(in)) die_perror("read input file failed");

    if (fseek(in, 0, SEEK_SET) != 0) die_perror("rewind input failed");

    Node *root = build_huffman_tree(freq);

    FILE *out = fopen(outfile, "wb");
    if (!out) {
        tree_free(root);
        fclose(in);
        die_perror("open compressed.log failed");
    }

    write_header(out, original_size, freq);

    if (original_size > 0) {
        Code codes[ALPHABET];
        for (int i = 0; i < ALPHABET; i++) code_clear(&codes[i]);
        Code empty; code_clear(&empty);
        build_codes_dfs(root, codes, &empty);

        BitWriter bw;
        bw_init(&bw, out);

        while ((ch = fgetc(in)) != EOF) {
            bw_write_code(&bw, &codes[(uint8_t)ch]);
        }
        if (ferror(in)) die_perror("read input during encoding failed");
        bw_flush(&bw);
    }

    if (fflush(out) != 0) die_perror("fflush output failed");

    uint64_t compressed_size = file_size_bytes(out);

    fclose(in);
    fclose(out);
    tree_free(root);

    printf("Original size   : %llu bytes\n", (unsigned long long)original_size);
    printf("Compressed size : %llu bytes\n", (unsigned long long)compressed_size);

    return EXIT_SUCCESS;
}
