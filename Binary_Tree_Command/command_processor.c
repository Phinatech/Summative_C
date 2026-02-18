#include "command_processor.h"
#include "fuzzy_match.h"
#include "file_ops.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/**
 * Initialize command processor
 */
void processor_init(CommandProcessor* processor) {
    processor->command_tree = NULL;
    processor->total_commands = 0;
    processor->commands_executed = 0;
    processor->commands_suggested = 0;
    processor->commands_rejected = 0;
}

/**
 * Load commands from file into BST
 */
bool processor_load_commands(CommandProcessor* processor, const char* filename) {
    processor->command_tree = load_commands_from_file(filename, &processor->total_commands);
    return processor->command_tree != NULL;
}

/**
 * Execute a command (or suggest/reject it)
 */
CommandResult processor_execute_command(CommandProcessor* processor, const char* command) {
    // Check for exact match
    if (bst_search(processor->command_tree, command)) {
        // EXACT MATCH - Execute command
        printf("\n");
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║              COMMAND AUTHORIZED                        ║\n");
        printf("╚════════════════════════════════════════════════════════╝\n");
        printf("✓ Executing: %s\n", command);
        printf("✓ Status: SUCCESS\n");
        printf("✓ Command completed successfully\n");
        printf("\n");
        
        processor->commands_executed++;
        return CMD_EXECUTED;
    }
    
    // No exact match - check for close matches
    BestMatch closest = find_closest_command(processor->command_tree, command);
    
    if (closest.found && should_suggest(closest.distance, strlen(command))) {
        // CLOSE MATCH - Suggest correction
        printf("\n");
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║              COMMAND NOT RECOGNIZED                    ║\n");
        printf("╚════════════════════════════════════════════════════════╝\n");
        printf("✗ Command entered: %s\n", command);
        printf("⚠ Did you mean: %s ?\n", closest.command);
        printf("  (Edit distance: %d)\n", closest.distance);
        printf("\n");
        printf("Please verify and re-enter the correct command.\n");
        printf("\n");
        
        processor->commands_suggested++;
        return CMD_SUGGESTED;
    }
    
    // UNRECOGNIZED - Reject and log
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║           COMMAND REJECTED - UNAUTHORIZED              ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("✗ Command: %s\n", command);
    printf("✗ Status: REJECTED\n");
    printf("✗ Command has been logged for security review\n");
    printf("\n");
    
    // Log to file
    log_rejected_command(command);
    
    processor->commands_rejected++;
    return CMD_REJECTED;
}

/**
 * Show command statistics
 */
void processor_show_statistics(const CommandProcessor* processor) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                  SYSTEM STATISTICS                        \n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Total approved commands: %d\n", processor->total_commands);
    printf("Commands executed:       %d\n", processor->commands_executed);
    printf("Commands suggested:      %d\n", processor->commands_suggested);
    printf("Commands rejected:       %d\n", processor->commands_rejected);
    printf("BST height (approx):     %d (balanced)\n", 
           (int)(1.44 * log2(processor->total_commands + 1)));
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");
}

/**
 * Callback for listing commands
 */
static void print_command(const char* command) {
    printf("  • %s\n", command);
}

/**
 * List all approved commands
 */
void processor_list_commands(const CommandProcessor* processor) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("              APPROVED COMMANDS (%d total)                 \n", 
           processor->total_commands);
    printf("═══════════════════════════════════════════════════════════\n");
    bst_inorder_traversal(processor->command_tree, print_command);
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");
}

/**
 * Clean up processor resources
 */
void processor_cleanup(CommandProcessor* processor) {
    if (processor->command_tree != NULL) {
        bst_free(processor->command_tree);
        processor->command_tree = NULL;
    }
}
