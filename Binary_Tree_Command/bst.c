#include "bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a new BST node
 */
BSTNode* bst_create_node(const char* command) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (node == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for BST node\n");
        return NULL;
    }
    
    strncpy(node->command, command, MAX_COMMAND_LENGTH - 1);
    node->command[MAX_COMMAND_LENGTH - 1] = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * Insert a command into the BST (case-sensitive)
 */
BSTNode* bst_insert(BSTNode* root, const char* command) {
    // Base case: empty tree
    if (root == NULL) {
        return bst_create_node(command);
    }
    
    // Compare commands (case-sensitive)
    int cmp = strcmp(command, root->command);
    
    if (cmp < 0) {
        // Insert in left subtree
        root->left = bst_insert(root->left, command);
    } else if (cmp > 0) {
        // Insert in right subtree
        root->right = bst_insert(root->right, command);
    }
    // If cmp == 0, command already exists, don't insert duplicate
    
    return root;
}

/**
 * Search for an exact command match in the BST
 */
bool bst_search(BSTNode* root, const char* command) {
    if (root == NULL) {
        return false;
    }
    
    int cmp = strcmp(command, root->command);
    
    if (cmp == 0) {
        return true;  // Exact match found
    } else if (cmp < 0) {
        return bst_search(root->left, command);
    } else {
        return bst_search(root->right, command);
    }
}

/**
 * Free all nodes in the BST
 */
void bst_free(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

/**
 * In-order traversal of BST (calls callback for each command)
 */
void bst_inorder_traversal(BSTNode* root, void (*callback)(const char*)) {
    if (root == NULL) {
        return;
    }
    
    bst_inorder_traversal(root->left, callback);
    callback(root->command);
    bst_inorder_traversal(root->right, callback);
}

/**
 * Count total nodes in BST
 */
int bst_count_nodes(BSTNode* root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + bst_count_nodes(root->left) + bst_count_nodes(root->right);
}

/**
 * Build a balanced BST from sorted array
 * This ensures O(log n) lookup time
 */
BSTNode* bst_build_balanced(char commands[][MAX_COMMAND_LENGTH], int start, int end) {
    if (start > end) {
        return NULL;
    }
    
    // Find middle element
    int mid = start + (end - start) / 2;
    
    // Create node with middle element
    BSTNode* node = bst_create_node(commands[mid]);
    if (node == NULL) {
        return NULL;
    }
    
    // Recursively build left and right subtrees
    node->left = bst_build_balanced(commands, start, mid - 1);
    node->right = bst_build_balanced(commands, mid + 1, end);
    
    return node;
}
