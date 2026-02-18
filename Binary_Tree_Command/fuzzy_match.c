#include "fuzzy_match.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Minimum of three values
static int min3(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

/**
 * Calculate Levenshtein distance between two strings
 * This measures the minimum number of single-character edits needed
 * to change one string into another
 */
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // Create distance matrix
    int** matrix = (int**)malloc((len1 + 1) * sizeof(int*));
    for (int i = 0; i <= len1; i++) {
        matrix[i] = (int*)malloc((len2 + 1) * sizeof(int));
    }
    
    // Initialize first column and row
    for (int i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        matrix[0][j] = j;
    }
    
    // Fill the matrix using dynamic programming
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            
            matrix[i][j] = min3(
                matrix[i-1][j] + 1,      // Deletion
                matrix[i][j-1] + 1,      // Insertion
                matrix[i-1][j-1] + cost  // Substitution
            );
        }
    }
    
    int distance = matrix[len1][len2];
    
    // Free matrix
    for (int i = 0; i <= len1; i++) {
        free(matrix[i]);
    }
    free(matrix);
    
    return distance;
}

/**
 * Helper function to traverse BST and find closest match
 */
static void find_best_match_helper(BSTNode* root, const char* input, BestMatch* best) {
    if (root == NULL) {
        return;
    }
    
    // Calculate distance to current node's command
    int distance = levenshtein_distance(input, root->command);
    
    // Update best match if this is closer
    if (distance < best->distance) {
        best->distance = distance;
        strncpy(best->command, root->command, MAX_COMMAND_LENGTH - 1);
        best->command[MAX_COMMAND_LENGTH - 1] = '\0';
        best->found = true;
    }
    
    // Traverse both subtrees
    find_best_match_helper(root->left, input, best);
    find_best_match_helper(root->right, input, best);
}

/**
 * Find the closest matching command in the BST
 */
BestMatch find_closest_command(BSTNode* root, const char* input) {
    BestMatch best;
    best.distance = INT_MAX;
    best.found = false;
    best.command[0] = '\0';
    
    find_best_match_helper(root, input, &best);
    
    return best;
}

/**
 * Determine if a suggestion should be made based on edit distance
 * and input length
 */
bool should_suggest(int distance, int input_length) {
    // Don't suggest if distance is too large
    if (distance > SUGGESTION_THRESHOLD) {
        return false;
    }
    
    // Don't suggest if the error rate is too high
    // (more than 40% of characters are wrong)
    if (input_length > 0 && distance * 100 / input_length > 40) {
        return false;
    }
    
    return true;
}
