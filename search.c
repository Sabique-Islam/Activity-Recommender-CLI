#include "include/search.h"

int main() {
    clear_screen();
    print_colored("Welcome to Activity Recommender CLI (ARC)\n", GREEN);
    print_colored("Loading embeddings and activities...\n", YELLOW);

    const char* embedding_file = "./data/glove.6B.300d.txt";

    FILE* test = fopen(embedding_file, "r");
    if (!test) {
        print_colored("Error: Could not open embeddings file\n", RED);
        exit(1);
    }
    fclose(test);

    HashMap* embedding_map = create_hashmap();

    load_embeddings(embedding_map, embedding_file);
    load_activities("./data/activities.csv");
    calculate_activity_vectors(embedding_map);

    print_colored("Ready! Type 'quit' to exit.\n", GREEN);

    char command[10];
    do {

        print_colored("\nEnter 'start' to begin or 'quit' to exit: ", BLUE);
        scanf("%s", command);
        getchar();

        lowercase_str(command);
        if (strcmp(command, "start") == 0) {
            process_group_preferences(embedding_map);
        }
        else if (strcmp(command, "quit") != 0) {
            print_colored("Invalid command. Please try again.\n", RED);
        }
    } while (strcmp(command, "quit") != 0);

    free_hashmap(embedding_map);
    cleanup_activities();
    print_colored("\nThank you for using ARC!\n", GREEN);
    
    return 0;
}