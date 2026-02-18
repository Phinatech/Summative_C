#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "bst.h"

// Command processing result
typedef enum {
    CMD_EXECUTED,
    CMD_SUGGESTED,
    CMD_REJECTED
} CommandResult;

// Command processor state
typedef struct {
    BSTNode* command_tree;
    int total_commands;
    int commands_executed;
    int commands_suggested;
    int commands_rejected;
} CommandProcessor;

// Function prototypes
void processor_init(CommandProcessor* processor);
bool processor_load_commands(CommandProcessor* processor, const char* filename);
CommandResult processor_execute_command(CommandProcessor* processor, const char* command);
void processor_show_statistics(const CommandProcessor* processor);
void processor_list_commands(const CommandProcessor* processor);
void processor_cleanup(CommandProcessor* processor);

#endif // COMMAND_PROCESSOR_H
