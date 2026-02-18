#include "file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Comparison function for qsort
 */
static int compare_commands(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

/**
 * Load approved commands from file and build balanced BST
 */
BSTNode* load_commands_from_file(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    // Array to temporarily store commands
    char commands[50][MAX_COMMAND_LENGTH];
    int cmd_count = 0;
    
    // Read commands from file
    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL && cmd_count < 50) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        // Copy command
        strncpy(commands[cmd_count], line, MAX_COMMAND_LENGTH - 1);
        commands[cmd_count][MAX_COMMAND_LENGTH - 1] = '\0';
        cmd_count++;
    }
    
    fclose(file);
    
    if (cmd_count == 0) {
        fprintf(stderr, "ERROR: No commands loaded from file\n");
        return NULL;
    }
    
    printf("Loaded %d approved commands from file\n", cmd_count);
    
    // Sort commands alphabetically for balanced BST construction
    qsort(commands, cmd_count, MAX_COMMAND_LENGTH, compare_commands);
    
    // Build balanced BST
    BSTNode* root = bst_build_balanced(commands, 0, cmd_count - 1);
    
    if (count != NULL) {
        *count = cmd_count;
    }
    
    return root;
}

/**
 * Log rejected command to file with timestamp
 */
bool log_rejected_command(const char* command) {
    FILE* file = fopen(REJECTED_LOG, "a");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open log file for writing\n");
        return false;
    }
    
    // Get current timestamp
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Write to log
    fprintf(file, "[%s] REJECTED: %s\n", timestamp, command);
    
    fclose(file);
    return true;
}

/**
 * Create a sample commands file for testing
 */
bool create_sample_commands_file(void) {
    FILE* file = fopen(COMMANDS_FILE, "w");
    if (file == NULL) {
        return false;
    }
    
    // Write sample industrial control commands
    const char* sample_commands[] = {
        "START_MOTOR",
        "STOP_MOTOR",
        "START_CONVEYOR",
        "STOP_CONVEYOR",
        "EMERGENCY_STOP",
        "RESET_ALARM",
        "CLEAR_FAULT",
        "START_PUMP",
        "STOP_PUMP",
        "OPEN_VALVE",
        "CLOSE_VALVE",
        "INCREASE_SPEED",
        "DECREASE_SPEED",
        "SET_TEMPERATURE",
        "CHECK_STATUS",
        "RUN_DIAGNOSTICS",
        "CALIBRATE_SENSOR",
        "ENABLE_HEATING",
        "DISABLE_HEATING",
        "START_COOLING",
        "STOP_COOLING",
        "ACTIVATE_ALARM",
        "DEACTIVATE_ALARM",
        "LOCK_SYSTEM",
        "UNLOCK_SYSTEM",
        "REBOOT_CONTROLLER",
        "SAVE_SETTINGS",
        "LOAD_SETTINGS",
        "EXPORT_DATA",
        "IMPORT_DATA",
        "START_BACKUP",
        "RESTORE_BACKUP",
        "ENABLE_AUTO_MODE",
        "DISABLE_AUTO_MODE",
        "SWITCH_TO_MANUAL",
        "INITIALIZE_SYSTEM",
        "SHUTDOWN_SYSTEM",
        "PAUSE_OPERATION",
        "RESUME_OPERATION",
        "ACKNOWLEDGE_ALERT"
    };
    
    int num_commands = sizeof(sample_commands) / sizeof(sample_commands[0]);
    
    for (int i = 0; i < num_commands; i++) {
        fprintf(file, "%s\n", sample_commands[i]);
    }
    
    fclose(file);
    printf("Created sample commands file: %s\n", COMMANDS_FILE);
    printf("Total commands: %d\n", num_commands);
    
    return true;
}
