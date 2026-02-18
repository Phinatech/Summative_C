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

/* ===================== Min-heap ===================== */
typedef struct {
    Node **a;
    int size, cap;
} MinHeap;

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

    if (h->size == 0) { heap_free(h); return NULL; }

    if (h->size == 1) {
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

/* ===================== Bit reader ===================== */
typedef struct {
    FILE *f;
    int cur;
    int pos; // 0..7
} BitReader;

static void br_init(BitReader *br, FILE *f) {
    br->f = f;
    br->cur = EOF;
    br->pos = 8;
}

static int br_read_bit(BitReader *br, int *out_bit) {
    if (br->pos >= 8) {
        br->cur = fgetc(br->f);
        if (br->cur == EOF) return 0;
        br->pos = 0;
    }
    *out_bit = (br->cur >> br->pos) & 1;
    br->pos++;
    return 1;
}

/* ===================== Header ===================== */
static void read_header(FILE *in, uint64_t *original_size, uint32_t freq[ALPHABET]) {
    char magic[4];
    if (fread(magic, 1, 4, in) != 4) die_perror("read magic failed");
    if (memcmp(magic, MAGIC, 4) != 0) {
        fprintf(stderr, "Error: Not a valid Huffman file (bad magic).\n");
        exit(EXIT_FAILURE);
    }
    if (fread(original_size, sizeof(*original_size), 1, in) != 1) die_perror("read original size failed");
    if (fread(freq, sizeof(uint32_t), ALPHABET, in) != ALPHABET) die_perror("read frequency table failed");
}

/* ===================== Verify (byte-for-byte + size) ===================== */
static int files_equal(const char *a_path, const char *b_path) {
    FILE *a = fopen(a_path, "rb");
    FILE *b = fopen(b_path, "rb");
    if (!a || !b) {
        if (a) fclose(a);
        if (b) fclose(b);
        return 0;
    }

    // quick size check first
    if (fseek(a, 0, SEEK_END) != 0 || fseek(b, 0, SEEK_END) != 0) {
        fclose(a); fclose(b);
        return 0;
    }
    long asz = ftell(a), bsz = ftell(b);
    if (asz < 0 || bsz < 0) { fclose(a); fclose(b); return 0; }
    if (asz != bsz) { fclose(a); fclose(b); return 0; }
    if (fseek(a, 0, SEEK_SET) != 0 || fseek(b, 0, SEEK_SET) != 0) {
        fclose(a); fclose(b);
        return 0;
    }

    int ca, cb;
    while (1) {
        ca = fgetc(a);
        cb = fgetc(b);
        if (ca != cb) { fclose(a); fclose(b); return 0; }
        if (ca == EOF) break;
    }

    fclose(a);
    fclose(b);
    return 1;
}

/* ===================== Main ===================== */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s <original_log.txt>\n"
                "Reads compressed.log -> writes decompressed.log -> verifies lossless\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char *original = argv[1];
    const char *infile = "compressed.log";
    const char *outfile = "decompressed.log";

    FILE *in = fopen(infile, "rb");
    if (!in) die_perror("open compressed.log failed");

    uint64_t original_size = 0;
    uint32_t freq[ALPHABET] = {0};
    read_header(in, &original_size, freq);

    Node *root = build_huffman_tree(freq);

    FILE *out = fopen(outfile, "wb");
    if (!out) {
        fclose(in);
        tree_free(root);
        die_perror("open decompressed.log failed");
    }

    if (original_size > 0) {
        if (!root) {
            fprintf(stderr, "Error: Corrupt header (original_size>0 but empty tree).\n");
            fclose(in); fclose(out);
            return EXIT_FAILURE;
        }

        BitReader br;
        br_init(&br, in);

        uint64_t written = 0;
        Node *cur = root;

        while (written < original_size) {
            if (cur->symbol >= 0) {
                if (fputc((uint8_t)cur->symbol, out) == EOF) die_perror("write decompressed byte failed");
                written++;
                cur = root;
                continue;
            }

            int bit;
            if (!br_read_bit(&br, &bit)) {
                fprintf(stderr, "Error: Unexpected end of compressed bitstream.\n");
                fclose(in); fclose(out); tree_free(root);
                return EXIT_FAILURE;
            }

            cur = bit ? cur->right : cur->left;
            if (!cur) {
                fprintf(stderr, "Error: Corrupted bitstream/tree (null traversal).\n");
                fclose(in); fclose(out); tree_free(root);
                return EXIT_FAILURE;
            }
        }
    }

    if (fflush(out) != 0) die_perror("fflush decompressed failed");

    fclose(in);
    fclose(out);
    tree_free(root);

    if (files_equal(original, outfile)) {
        printf("✅ Integrity check PASSED: decompressed.log matches original exactly.\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ Integrity check FAILED: decompressed.log differs from original.\n");
        return EXIT_FAILURE;
    }
}
