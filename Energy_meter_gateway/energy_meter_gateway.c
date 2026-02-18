#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

// Maximum number of events in the buffer
#define MAX_EVENTS 20

// Event types
typedef enum {
    EVENT_POWER_CONSUMPTION,
    EVENT_VOLTAGE_LEVEL,
    EVENT_FREQUENCY_STABILITY,
    EVENT_FAULT_ALERT
} EventType;

// Event data structure
typedef struct {
    EventType type;
    double value;
    time_t timestamp;
    char description[100];
} EventData;

// Doubly linked list node
typedef struct EventNode {
    EventData data;
    struct EventNode* prev;
    struct EventNode* next;
} EventNode;

// Gateway system state
typedef struct {
    EventNode* head;           // Oldest event
    EventNode* tail;           // Newest event
    EventNode* cursor;         // Current position for navigation
    int event_count;           // Total events in the list
    bool live_mode;            // Live display active
    bool running;              // System running flag
} GatewayState;

// Global gateway state
GatewayState gateway;

// Function prototypes
void initialize_system();
void add_event(EventType type, double value, const char* description);
void remove_oldest_event();
void display_event(EventNode* node);
void navigate_next();
void navigate_previous();
void start_live_display();
void pause_live_display();
void clear_all_events();
void terminate_system();
void simulate_incoming_events();
void process_command(char command);
const char* event_type_to_string(EventType type);

/**
 * Initialize the gateway system
 */
void initialize_system() {
    gateway.head = NULL;
    gateway.tail = NULL;
    gateway.cursor = NULL;
    gateway.event_count = 0;
    gateway.live_mode = false;
    gateway.running = true;
    
    printf("=== Smart Energy Meter Gateway Initialized ===\n");
    printf("Memory-constrained mode: Maximum %d events\n", MAX_EVENTS);
    printf("Cursor starting position: Oldest event\n\n");
}

/**
 * Convert event type to string
 */
const char* event_type_to_string(EventType type) {
    switch(type) {
        case EVENT_POWER_CONSUMPTION: return "POWER_CONSUMPTION";
        case EVENT_VOLTAGE_LEVEL: return "VOLTAGE_LEVEL";
        case EVENT_FREQUENCY_STABILITY: return "FREQUENCY_STABILITY";
        case EVENT_FAULT_ALERT: return "FAULT_ALERT";
        default: return "UNKNOWN";
    }
}

/**
 * Add a new event to the doubly linked list
 * Implements circular buffer behavior (removes oldest when full)
 */
void add_event(EventType type, double value, const char* description) {
    // Check if buffer is full - remove oldest event
    if (gateway.event_count >= MAX_EVENTS) {
        printf("[BUFFER FULL] Removing oldest event to make space...\n");
        remove_oldest_event();
    }
    
    // Allocate new node
    EventNode* new_node = (EventNode*)malloc(sizeof(EventNode));
    if (new_node == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed!\n");
        return;
    }
    
    // Initialize event data
    new_node->data.type = type;
    new_node->data.value = value;
    new_node->data.timestamp = time(NULL);
    strncpy(new_node->data.description, description, 99);
    new_node->data.description[99] = '\0';
    new_node->next = NULL;
    new_node->prev = NULL;
    
    // Insert into doubly linked list
    if (gateway.head == NULL) {
        // First event - initialize list
        gateway.head = new_node;
        gateway.tail = new_node;
        gateway.cursor = new_node;  // Cursor starts at oldest event
    } else {
        // Append to tail (newest events at the end)
        new_node->prev = gateway.tail;
        gateway.tail->next = new_node;
        gateway.tail = new_node;
    }
    
    gateway.event_count++;
    
    // Display event if in live mode
    if (gateway.live_mode) {
        printf("\n[NEW EVENT] ");
        display_event(new_node);
    }
}

/**
 * Remove the oldest event (head of the list)
 */
void remove_oldest_event() {
    if (gateway.head == NULL) {
        return;  // Empty list
    }
    
    EventNode* old_head = gateway.head;
    
    // Update cursor if it points to the node being deleted
    if (gateway.cursor == old_head) {
        gateway.cursor = old_head->next;  // Move cursor to next event
    }
    
    // Update head pointer
    gateway.head = old_head->next;
    
    if (gateway.head != NULL) {
        gateway.head->prev = NULL;
    } else {
        // List is now empty
        gateway.tail = NULL;
        gateway.cursor = NULL;
    }
    
    // Free memory
    free(old_head);
    gateway.event_count--;
}

/**
 * Display an event with formatted output
 */
void display_event(EventNode* node) {
    if (node == NULL) {
        printf("No event to display.\n");
        return;
    }
    
    char time_str[26];
    struct tm* tm_info = localtime(&node->data.timestamp);
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    printf("┌─────────────────────────────────────────────────────────\n");
    printf("│ Type: %-20s  Value: %.2f\n", 
           event_type_to_string(node->data.type), node->data.value);
    printf("│ Time: %s\n", time_str);
    printf("│ Description: %s\n", node->data.description);
    printf("└─────────────────────────────────────────────────────────\n");
}

/**
 * Navigate to the next event (newer)
 */
void navigate_next() {
    if (gateway.cursor == NULL) {
        printf("No events available.\n");
        return;
    }
    
    if (gateway.cursor->next != NULL) {
        gateway.cursor = gateway.cursor->next;
        printf("\n[NAVIGATE NEXT]\n");
        display_event(gateway.cursor);
    } else {
        printf("Already at the newest event.\n");
    }
}

/**
 * Navigate to the previous event (older)
 */
void navigate_previous() {
    if (gateway.cursor == NULL) {
        printf("No events available.\n");
        return;
    }
    
    if (gateway.cursor->prev != NULL) {
        gateway.cursor = gateway.cursor->prev;
        printf("\n[NAVIGATE PREVIOUS]\n");
        display_event(gateway.cursor);
    } else {
        printf("Already at the oldest event.\n");
    }
}

/**
 * Start live event display mode
 */
void start_live_display() {
    gateway.live_mode = true;
    printf("\n=== LIVE EVENT DISPLAY MODE ACTIVATED ===\n");
    printf("New events will be displayed as they arrive.\n");
    printf("Press 'h' to pause live display.\n\n");
}

/**
 * Pause live event display
 */
void pause_live_display() {
    gateway.live_mode = false;
    printf("\n=== LIVE DISPLAY PAUSED ===\n");
    printf("Event collection continues in background.\n");
    printf("Press 'r' to resume live display.\n\n");
}

/**
 * Clear all events and free memory
 */
void clear_all_events() {
    printf("\nClearing all events...\n");
    
    EventNode* current = gateway.head;
    while (current != NULL) {
        EventNode* next = current->next;
        free(current);
        current = next;
    }
    
    gateway.head = NULL;
    gateway.tail = NULL;
    gateway.cursor = NULL;
    gateway.event_count = 0;
    
    printf("All events cleared. Memory freed.\n");
    printf("Event count: %d\n\n", gateway.event_count);
}

/**
 * Safely terminate the system and store session state
 */
void terminate_system() {
    printf("\n=== SYSTEM TERMINATION INITIATED ===\n");
    
    // Store session state (in a real system, this would write to persistent storage)
    printf("Storing session state...\n");
    printf("Total events logged: %d\n", gateway.event_count);
    
    if (gateway.cursor != NULL) {
        printf("Current cursor position saved.\n");
    }
    
    // Clean up memory
    clear_all_events();
    
    gateway.running = false;
    printf("System terminated safely.\n");
}

/**
 * Simulate incoming meter events for demonstration
 */
void simulate_incoming_events() {
    static int event_counter = 0;
    
    // Generate random events
    EventType types[] = {
        EVENT_POWER_CONSUMPTION,
        EVENT_VOLTAGE_LEVEL,
        EVENT_FREQUENCY_STABILITY,
        EVENT_FAULT_ALERT
    };
    
    const char* descriptions[] = {
        "Meter #101: Peak load detected",
        "Meter #102: Voltage fluctuation",
        "Meter #103: Frequency deviation 0.5Hz",
        "Meter #104: Critical fault - phase imbalance"
    };
    
    EventType type = types[event_counter % 4];
    double value = 100.0 + (rand() % 100);
    
    add_event(type, value, descriptions[event_counter % 4]);
    event_counter++;
}

/**
 * Process operator commands
 */
void process_command(char command) {
    switch(command) {
        case 'n':
            navigate_next();
            break;
        case 'p':
            navigate_previous();
            break;
        case 'r':
            start_live_display();
            break;
        case 'h':
            pause_live_display();
            break;
        case 'c':
            clear_all_events();
            break;
        case 'x':
            terminate_system();
            break;
        case 'd':
            // Display current event
            printf("\n[CURRENT EVENT]\n");
            display_event(gateway.cursor);
            break;
        case 's':
            // Show system status
            printf("\n=== SYSTEM STATUS ===\n");
            printf("Total events: %d / %d\n", gateway.event_count, MAX_EVENTS);
            printf("Live mode: %s\n", gateway.live_mode ? "ACTIVE" : "PAUSED");
            printf("Cursor position: %s\n", 
                   gateway.cursor ? "Valid" : "No events");
            printf("====================\n\n");
            break;
        case '?':
            printf("\n=== COMMAND HELP ===\n");
            printf("n - Navigate to next event (newer)\n");
            printf("p - Navigate to previous event (older)\n");
            printf("r - Start live event display\n");
            printf("h - Pause live display\n");
            printf("c - Clear all events\n");
            printf("x - Terminate system\n");
            printf("d - Display current event\n");
            printf("s - Show system status\n");
            printf("? - Show this help\n");
            printf("====================\n\n");
            break;
        default:
            printf("Unknown command '%c'. Press '?' for help.\n", command);
    }
}

/**
 * Main program entry point
 */
int main() {
    char command;
    int event_timer = 0;
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize the gateway system
    initialize_system();
    
    // Add some initial events for demonstration
    printf("Loading initial events...\n");
    add_event(EVENT_POWER_CONSUMPTION, 1250.5, "Meter #101: Normal operation");
    add_event(EVENT_VOLTAGE_LEVEL, 230.2, "Meter #102: Voltage nominal");
    add_event(EVENT_FREQUENCY_STABILITY, 50.0, "Meter #103: Frequency stable");
    printf("Initial events loaded. Count: %d\n\n", gateway.event_count);
    
    // Display first event (cursor starts at oldest)
    printf("Displaying oldest event:\n");
    display_event(gateway.cursor);
    
    // Print command help
    printf("\n=== COMMAND MENU ===\n");
    printf("n - Next event | p - Previous event | r - Live mode\n");
    printf("h - Pause live | c - Clear all | x - Exit | ? - Help\n");
    printf("====================\n\n");
    
    // Main event loop
    printf("Enter command: ");
    fflush(stdout);
    
    while (gateway.running) {
        // Simulate periodic event arrival (every 5 iterations in demo mode)
        event_timer++;
        if (event_timer >= 5 && gateway.event_count < MAX_EVENTS) {
            simulate_incoming_events();
            event_timer = 0;
            
            if (!gateway.live_mode) {
                printf("\n[Background] New event logged (use 'r' to view live)\n");
                printf("Enter command: ");
                fflush(stdout);
            }
        }
        
        // Non-blocking input simulation (in real embedded system, use interrupt-driven I/O)
        // For demonstration, we'll use a simple blocking getchar
        command = getchar();
        
        // Skip newline characters
        if (command == '\n' || command == '\r') {
            continue;
        }
        
        // Process the command
        process_command(command);
        
        if (gateway.running) {
            printf("Enter command: ");
            fflush(stdout);
        }
        
        // Small delay to prevent tight loop in demo
        usleep(100000);  // 100ms
    }
    
    return 0;
}
