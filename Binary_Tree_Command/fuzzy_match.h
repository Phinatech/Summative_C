#ifndef FUZZY_MATCH_H
#define FUZZY_MATCH_H

#include "bst.h"

// Threshold for suggesting commands (edit distance)
#define SUGGESTION_THRESHOLD 3

// Structure to hold best match result
typedef struct {
    char command[MAX_COMMAND_LENGTH];
    int distance;
    bool found;
} BestMatch;

// Levenshtein distance calculation
int levenshtein_distance(const char* s1, const char* s2);

// Find closest matching command in BST
BestMatch find_closest_command(BSTNode* root, const char* input);

// Helper function to check if suggestion should be made
bool should_suggest(int distance, int input_length);

#endif // FUZZY_MATCH_H
