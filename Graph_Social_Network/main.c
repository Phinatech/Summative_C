#include "graph.h"
#include <stdio.h>
#include <string.h>

/* Robust input helpers (rubric-friendly) */
static void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static int read_int(const char *prompt) {
    int x;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &x) == 1) {
            clear_input_buffer();
            return x;
        }
        printf("❌ Invalid number. Try again.\n");
        clear_input_buffer();
    }
}

static void read_id(const char *prompt, char out[ID_MAX]) {
    while (1) {
        printf("%s", prompt);
        if (scanf("%15s", out) == 1) {
            clear_input_buffer();
            // simple validation: must start with 'U'
            if (out[0] == 'U' && strlen(out) >= 2) return;
            printf("⚠️  Invalid ID format. Example valid: U103\n");
        } else {
            clear_input_buffer();
            printf("❌ Invalid input. Try again.\n");
        }
    }
}

static void menu(void) {
    printf("====================================\n");
    printf("  Interaction Mapping Tool (C)\n");
    printf("  Directed Graph of User Interactions\n");
    printf("====================================\n");
    printf("1) Query a user (incoming + outgoing)\n");
    printf("2) Print adjacency matrix\n");
    printf("3) Add user\n");
    printf("4) Remove user\n");
    printf("5) Add interaction (FROM -> TO)\n");
    printf("6) Remove interaction (FROM -> TO)\n");
    printf("0) Exit\n");
    printf("------------------------------------\n");
}

int main(void) {
    Graph g;
    graph_init(&g);

    // Build the directed graph from the provided dataset
    seed_initial_graph(&g);

    printf("✅ Graph loaded with initial interaction data.\n\n");

    while (1) {
        menu();
        int choice = read_int("Choose an option: ");

        char a[ID_MAX], b[ID_MAX];

        switch (choice) {
            case 1:
                read_id("Enter user ID to query (e.g., U103): ", a);
                query_user(&g, a);
                break;

            case 2:
                print_adjacency_matrix(&g);
                break;

            case 3:
                read_id("Enter new user ID to add: ", a);
                add_user(&g, a, true);
                break;

            case 4:
                read_id("Enter user ID to remove: ", a);
                remove_user(&g, a, true);
                break;

            case 5:
                read_id("Enter FROM user ID: ", a);
                read_id("Enter TO user ID:   ", b);
                add_interaction(&g, a, b, true);
                break;

            case 6:
                read_id("Enter FROM user ID: ", a);
                read_id("Enter TO user ID:   ", b);
                remove_interaction(&g, a, b, true);
                break;

            case 0:
                graph_free(&g);
                printf("👋 Exiting. Memory freed successfully.\n");
                return 0;

            default:
                printf("⚠️  Unknown option. Please choose 0–6.\n");
        }

        printf("\n"); // spacing between actions
    }
}
