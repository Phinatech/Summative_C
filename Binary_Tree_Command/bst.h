#ifndef BST_H
#define BST_H

#include <stdbool.h>

// Maximum command length
#define MAX_COMMAND_LENGTH 50

// BST Node structure
typedef struct BSTNode {
    char command[MAX_COMMAND_LENGTH];
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

// BST operations
BSTNode* bst_create_node(const char* command);
BSTNode* bst_insert(BSTNode* root, const char* command);
bool bst_search(BSTNode* root, const char* command);
void bst_free(BSTNode* root);
void bst_inorder_traversal(BSTNode* root, void (*callback)(const char*));
int bst_count_nodes(BSTNode* root);

// Balanced BST construction
BSTNode* bst_build_balanced(char commands[][MAX_COMMAND_LENGTH], int start, int end);

#endif // BST_H
