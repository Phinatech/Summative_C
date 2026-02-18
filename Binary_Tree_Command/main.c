#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "command_processor.h"
#include "file_ops.h"

#define INPUT_BUFFER_SIZE 100

/**
 * Display welcome screen
 */
void display_welcome(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║     INDUSTRIAL CONTROL TERMINAL                        ║\n");
    printf("║     Command Authorization System v1.0                  ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Security Features:\n");
    printf("  • Binary Search Tree for O(log n) command lookup\n");
    printf("  • Case-sensitive exact matching required\n");
    printf("  • Typo detection with Levenshtein distance\n");
    printf("  • Automatic logging of unauthorized commands\n");
    printf("\n");
}

/**
 * Display help menu
 */
void display_help(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                     HELP MENU                             \n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Special Commands:\n");
    printf("  list      - Display all approved commands\n");
    printf("  stats     - Show system statistics\n");
    printf("  help      - Display this help menu\n");
    printf("  clear     - Clear the screen\n");
    printf("  exit      - Exit the terminal\n");
    printf("\n");
    printf("Control Commands:\n");
    printf("  Enter any approved command exactly as specified\n");
    printf("  Commands are case-sensitive (e.g., START_MOTOR)\n");
    printf("  System will suggest corrections for minor typos\n");
    printf("  Unrecognized commands are logged for review\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");
}

/**
 * Trim whitespace from string
 */
void trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while (*str == ' ' || *str == '\t') str++;
    
    // All spaces?
    if (*str == 0) return;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    
    // Write new null terminator
    *(end + 1) = '\0';
}

/**
 * Main program entry point
 */
int main(void) {
    CommandProcessor processor;
    char input[INPUT_BUFFER_SIZE];
    bool running = true;
    
    // Display welcome screen
    display_welcome();
    
    // Initialize processor
    processor_init(&processor);
    
    // Check if commands file exists, create if not
    FILE* test = fopen(COMMANDS_FILE, "r");
    if (test == NULL) {
        printf("Commands file not found. Creating sample file...\n");
        if (!create_sample_commands_file()) {
            fprintf(stderr, "ERROR: Failed to create sample commands file\n");
            return 1;
        }
    } else {
        fclose(test);
    }
    
    // Load approved commands from file
    printf("Loading approved commands...\n");
    if (!processor_load_commands(&processor, COMMANDS_FILE)) {
        fprintf(stderr, "ERROR: Failed to load commands\n");
        return 1;
    }
    
    printf("System initialized successfully.\n");
    printf("Type 'help' for available commands.\n");
    printf("\n");
    
    // Main command loop
    while (running) {
        printf("Terminal> ");
        fflush(stdout);
        
        // Read input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline and whitespace
        trim_whitespace(input);
        
        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }
        
        // Handle special commands
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("\nShutting down terminal...\n");
            running = false;
            continue;
        }
        
        if (strcmp(input, "help") == 0) {
            display_help();
            continue;
        }
        
        if (strcmp(input, "list") == 0) {
            processor_list_commands(&processor);
            continue;
        }
        
        if (strcmp(input, "stats") == 0) {
            processor_show_statistics(&processor);
            continue;
        }
        
        if (strcmp(input, "clear") == 0) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            display_welcome();
            continue;
        }
        
        // Process control command
        processor_execute_command(&processor, input);
    }
    
    // Show final statistics
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                 SESSION SUMMARY                           \n");
    printf("═══════════════════════════════════════════════════════════\n");
    processor_show_statistics(&processor);
    
    // Cleanup
    processor_cleanup(&processor);
    
    printf("Terminal closed safely.\n");
    printf("\n");
    
    return 0;
}
