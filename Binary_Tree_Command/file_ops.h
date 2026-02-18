#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "bst.h"

// Default file paths
#define COMMANDS_FILE "approved_commands.txt"
#define REJECTED_LOG "rejected_commands.log"

// Function prototypes
BSTNode* load_commands_from_file(const char* filename, int* count);
bool log_rejected_command(const char* command);
bool create_sample_commands_file(void);

#endif // FILE_OPS_H
