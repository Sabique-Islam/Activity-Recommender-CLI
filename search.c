// Header files

#include <stdio.h>      // For printf, scanf
#include <stdlib.h>     // For malloc, free
#include <string.h>     // For strcpy, strcmp
#include <math.h>       // For sqrt
#include <ctype.h>      // For tolower
#include <unistd.h>     // For POSIX operating system API like access
#include <errno.h>      // For error handling
#include "include/utils.h"

// MACRO Definitions

// ANSI color code
#define RED "\033[0;31m"     // Red
#define GREEN "\033[0;32m"   // Green
#define YELLOW "\033[0;33m"  // Yellow
#define BLUE "\033[0;34m"    // Blue
#define RESET "\033[0m"      // Reset text color to default

#define INITIAL_CAPACITY 1000    // Starting with space for 1000 activities
#define GROWTH_FACTOR 2          // Double the size when more space is needed

Activity* activities = NULL;      // Pointer to array of activities
int activity_count = 0;           // Number of activities loaded
int activities_capacity = 0;      // Current capacity of activities array

#define MAX_LINE_LEN 8192

void load_embeddings(HashMap* map, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        print_colored("\nError: Could not open embeddings file: ", RED);
        printf("%s\n", filename);
        exit(1);
    }
    print_colored("Loading embeddings... ", YELLOW);
    
    // Buffer for reading lines
    char* line = malloc(MAX_LINE_LEN);
    if (!line) {
        print_colored("Error: Memory allocation failed\n", RED);
        fclose(file);
        exit(1);
    }
    
    char word[MAX_WORD_LEN];
    float vector[EMBEDDING_DIM];
    int count = 0;
    int line_count = 0;
    
    // Read file line by line
    while (fgets(line, MAX_LINE_LEN, file)) {
        line_count++;
        
        // Check if line was fully read
        size_t len = strlen(line);
        if (len == MAX_LINE_LEN - 1 && line[len-1] != '\n') {
            // Read until end of line
            int c;
            while ((c = fgetc(file)) != EOF && c != '\n');
            continue;
        }
        
        // Remove newline (if present)
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }
        
        // Skip empty lines
        if (len == 0){
             continue;
        }
        
        // Find the first space to separate word from vector
        char* space = strchr(line, ' ');
        if (!space) continue;
        
        // Extract the word
        size_t word_len = space - line;
        if (word_len >= MAX_WORD_LEN) {
            word_len = MAX_WORD_LEN - 1;
        }
        strncpy(word, line, word_len);
        word[word_len] = '\0';
        lowercase_str(word);
        
        // Parse the vector values
        char* pos = space + 1;
        int dim_count = 0;
        char* endptr;
        
        while (dim_count < EMBEDDING_DIM && *pos) {
            // Skip whitespace
            while (*pos && isspace(*pos)) pos++;
            if (!*pos) 
                break;
            
            // Parse number
            vector[dim_count] = strtof(pos, &endptr);
            
            // Check if conversion was successful
            if (pos == endptr) 
                break;
            
            dim_count++;
            pos = endptr;
        }
        
        // Check if we got all dimensions
        if (dim_count != EMBEDDING_DIM) continue;
        
        // Normalize the vector
        float magnitude = 0.0;
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            magnitude += vector[i] * vector[i];
        }
        magnitude = sqrt(magnitude);
        
        if (magnitude > 1e-10) {
            for (int i = 0; i < EMBEDDING_DIM; i++) {
                vector[i] /= magnitude;
            }
            
            // Add to hash map
            insert_embedding(map, word, vector);
            count++;
            
            // Print progress
            if (count % 50000 == 0) {
                printf(".");
                fflush(stdout);
            }
        }
    }
    
    free(line);
    printf("\nLoaded %d words from embeddings file.\n", count);
    fclose(file);
}

// Add this helper function to dump the first few entries in the hash map
void dump_hashmap_sample(HashMap* map) {
    printf("\nSample of words in the hash map:\n");
    int count = 0;
    
    for (int i = 0; i < HASH_SIZE && count < 20; i++) {
        Node* current = map->table[i];
        while (current != NULL && count < 20) {
            printf("Word: '%s'\n", current->data.word);
            count++;
            current = current->next;
        }
    }
}

// Helper function to preprocess tags
void preprocess_tag(char* tag) {
    // Convert to lowercase
    lowercase_str(tag);

    // Replace hyphens with spaces for compound words
    // Handle words like "skill-based" -> "skill based"
    for (int i = 0; tag[i]; i++) {
        if (tag[i] == '-') {
            tag[i] = ' ';
        }
    }
}

void load_activities(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        print_colored("Error: Could not open activities file\n", RED);
        exit(1);
    }

    activities_capacity = INITIAL_CAPACITY;  // Start with space for 1000 activities
    activities = malloc(activities_capacity * sizeof(Activity));  // Allocate memory

    if (!activities) {
        print_colored("Error: Memory allocation failed\n", RED);
        fclose(file);
        exit(1);
    }
    activity_count = 0;  // Start with 0 activities

    char line[MAX_LINE_LEN];  // Buffer to store each line from the file

    fgets(line, sizeof(line), file); // To skip header line

    while (fgets(line, sizeof(line), file)) {

        // Check if we need to grow the array
        if (activity_count >= activities_capacity) {

            // Double the capacity
            activities_capacity *= GROWTH_FACTOR;

            // Reallocate memory with the new size
            Activity* temp = realloc(activities, activities_capacity * sizeof(Activity));

            if (!temp) {
                // If memory reallocation fails, print an error message and exit
                print_colored("Error: Memory reallocation failed\n", RED);
                free(activities);
                fclose(file);
                exit(1);
            }
            activities = temp;  // Update our pointer to the new memory
        }

        // Parse the first token (activity name)
        char* token = strtok(line, ",");
        if (!token) continue;  // Skip if no token found

        // Copy the activity name
        strncpy(activities[activity_count].name, token, MAX_WORD_LEN - 1);
        activities[activity_count].name[MAX_WORD_LEN - 1] = '\0';  // Ensure null termination
        activities[activity_count].tag_count = 0;  // Start with 0 tags

        // Parse the remaining tokens (tags)
        while ((token = strtok(NULL, ",\n")) && activities[activity_count].tag_count < MAX_TAGS) {
            // Create a lowercase copy of the tag
            char lowercase_token[MAX_WORD_LEN];
            strncpy(lowercase_token, token, MAX_WORD_LEN - 1);
            lowercase_token[MAX_WORD_LEN - 1] = '\0';  // Ensure null termination
            lowercase_str(lowercase_token);  // Convert to lowercase

            // Copy the tag to our activity
            strncpy(activities[activity_count].tags[activities[activity_count].tag_count],
                   lowercase_token, MAX_WORD_LEN - 1);
            activities[activity_count].tag_count++;  // Increment tag count
        }

        activity_count++;  // Increment activity count
    }

    // Shrink the array to the actual size if possible
    if (activity_count < activities_capacity) {
        Activity* temp = realloc(activities, activity_count * sizeof(Activity));

        if (temp) {
            activities = temp;  // Update our pointer to the new memory
            activities_capacity = activity_count;  // Update capacity
        }
    }

    fclose(file);  // Close the file
    printf("Loaded %d activities\n", activity_count);
}

void calculate_activity_vectors(HashMap* map) {

    // For each activity, calculate its average vector based on its tags
    for (int i = 0; i < activity_count; i++) {

        // Initialize the average vector to all zeros
        memset(activities[i].avg_vector, 0, sizeof(float) * EMBEDDING_DIM);
        int valid_tags = 0;  // Count how many tags have valid vectors

        // Process each tag
        for (int j = 0; j < activities[i].tag_count; j++) {

            // Find the tag in the embeddings
            WordEmbedding* embedding = find_embedding(map, activities[i].tags[j]);
            if (embedding) {
                // Check for invalid values (NaN or infinity)

                int valid_vector = 1;
                for (int k = 0; k < EMBEDDING_DIM; k++) {
                    if (isnan(embedding->vector[k]) || isinf(embedding->vector[k])) {
                        valid_vector = 0;
                        break;
                    }
                }

                // If the vector is valid, add it to the average

                if (valid_vector) {
                    for (int k = 0; k < EMBEDDING_DIM; k++) {
                        activities[i].avg_vector[k] += embedding->vector[k];
                    }
                    valid_tags++;
                }
            } else {
                // If the tag isn't found, split it into individual words

                char tag_copy[MAX_WORD_LEN];
                strncpy(tag_copy, activities[i].tags[j], MAX_WORD_LEN - 1);
                tag_copy[MAX_WORD_LEN - 1] = '\0';  // NULL termination

                // Replace hyphens with spaces for compound words

                for (int k = 0; tag_copy[k]; k++) {
                    if (tag_copy[k] == '-') {
                        tag_copy[k] = ' ';
                    }
                }

                char* word;
                int word_count = 0;
                float temp_vector[EMBEDDING_DIM] = {0};  // Temporary vector for the not found tag before splitting

                // Split the tag into words
                word = strtok(tag_copy, " ");

                while (word) {

                    if (strlen(word) > 0) {
                        // Finding each word in the embeddings
                        WordEmbedding* sub_embedding = find_embedding(map, word);

                        if (sub_embedding) {
                            // Add to temporary vector
                            for (int k = 0; k < EMBEDDING_DIM; k++) {
                                temp_vector[k] += sub_embedding->vector[k];
                            }
                            word_count++;
                        }
                    }
                    word = strtok(NULL, " ");  // Get next word
                }

                // If vectors are found for any words, add their average to the activity vector
                if (word_count > 0) {
                    // Average the vectors of individual words
                    for (int k = 0; k < EMBEDDING_DIM; k++) {
                        temp_vector[k] /= word_count;
                        activities[i].avg_vector[k] += temp_vector[k];
                    }
                    valid_tags++;
                }
            }
        }

        // Calculate the final average vector
        if (valid_tags > 0) {
            for (int k = 0; k < EMBEDDING_DIM; k++) {
                activities[i].avg_vector[k] /= valid_tags;
            }
        } else {
            // Initialize with small non-zero values to avoid NaN in similarity
            for (int k = 0; k < EMBEDDING_DIM; k++) {
                activities[i].avg_vector[k] = 1e-6;
            }
        }
    }
}

void process_group_preferences(HashMap* map) {
    int group_size;
    print_colored("\nEnter number of people in the group: ", BLUE);
    scanf("%d", &group_size);
    getchar(); // Consume newline character left by scanf

    // Initialize the group vector to all zeros
    float group_vector[EMBEDDING_DIM] = {0};
    int total_valid_words = 0;  // Count how many valid words are processed

    // Process preferences for each person
    for (int i = 0; i < group_size; i++) {

        // Ask for each person's preferences
        print_colored("\nPerson ", BLUE);
        printf("%d", i + 1);
        print_colored(", enter your preferences (space-separated): ", BLUE);

        // Get their input
        char preferences[MAX_LINE_LEN];
        fgets(preferences, sizeof(preferences), stdin);
        preferences[strcspn(preferences, "\n")] = 0;  // Remove newline character
        
        lowercase_str(preferences);  // Convert to lowercase
        remove_punctuation(preferences);  // Remove punctuation

        // Process each word in the preference
        char* word = strtok(preferences, " ");
        while (word) {
            if (strlen(word) > 0) {  // Skip empty strings
                if (!is_stopword(word)) {  // Skip common words

                    // Try to find the word in the embeddings
                    WordEmbedding* embedding = find_embedding(map, word);
                    if (embedding) {
                        // Add this word's vector to the group vector
                        for (int j = 0; j < EMBEDDING_DIM; j++) {
                            group_vector[j] += embedding->vector[j];
                        }
                        total_valid_words++;
                    } else {

                        // Split compound words with hyphens
                        char word_copy[MAX_WORD_LEN];
                        strncpy(word_copy, word, MAX_WORD_LEN - 1);
                        word_copy[MAX_WORD_LEN - 1] = '\0';  // NULL termination

                        // Replace hyphens with spaces
                        int has_hyphen = 0;
                        for (int k = 0; word_copy[k]; k++) {
                            if (word_copy[k] == '-') {
                                word_copy[k] = ' ';
                                has_hyphen = 1;
                            }
                        }

                        // If the word had hyphens, try to process its parts
                        if (has_hyphen) {
                            char* subword;
                            int subword_count = 0;
                            float temp_vector[EMBEDDING_DIM] = {0};  // Temporary vector for this word

                            // Split the word into subwords
                            subword = strtok(word_copy, " ");
                            while (subword) {
                                if (strlen(subword) > 0 && !is_stopword(subword)) {
                                
                                    WordEmbedding* sub_embedding = find_embedding(map, subword);
                                    if (sub_embedding) {

                                        for (int j = 0; j < EMBEDDING_DIM; j++) {
                                            temp_vector[j] += sub_embedding->vector[j];
                                        }
                                        subword_count++;
                                    }
                                }
                                subword = strtok(NULL, " ");  // Get next subword
                            }

                            // If vectors are found for any subwords, add their average to the group vector
                            if (subword_count > 0) {
                                for (int j = 0; j < EMBEDDING_DIM; j++) {
                                    temp_vector[j] /= subword_count;
                                    group_vector[j] += temp_vector[j];
                                }
                                total_valid_words++;
                            }
                        }
                    }
                }
            }
            word = strtok(NULL, " ");  // Get next word
        }
    }

    // Calculate final average group vector
    if (total_valid_words > 0) {
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            group_vector[i] /= total_valid_words;
        }

        // Find top 3 activities that match the group vector
        typedef struct {
            int index;  // Index of the activity in the activities array
            float similarity;  // Similarity score
        } ActivityMatch;

        // Initialize top 3 matches with invalid values
        ActivityMatch top3[3] = {{-1, -1}, {-1, -1}, {-1, -1}};

        // Compare the group vector with each activity vector
        for (int i = 0; i < activity_count; i++) {

            // Calculate cosine similarity between group vector and activity vector
            float sim = cosine_similarity(group_vector, activities[i].avg_vector);

            // Check if this activity should be in our top 3
            for (int j = 0; j < 3; j++) {
                if (sim > top3[j].similarity) {
                    // Shift lower matches down
                    for (int k = 2; k > j; k--) {
                        top3[k] = top3[k-1];
                    }
                    // Insert this match
                    top3[j].index = i;
                    top3[j].similarity = sim;
                    break;
                }
            }
        }

        // Display the top 3 matches
        print_colored("\nTop 3 recommended activities:\n", GREEN);
        for (int i = 0; i < 3; i++) {
            if (top3[i].index != -1) {
                printf("%d. %s (%.2f%% match)\n",
                       i + 1,
                       activities[top3[i].index].name,
                       top3[i].similarity * 100);  // Percentage
            }
        }
    } else {
        print_colored("No valid preferences found in embeddings.\n", RED);
    }
}

void cleanup_activities() {
    // Free the memory allocated for activities
    if (activities) {
        free(activities);
        activities = NULL;
        activity_count = 0;
        activities_capacity = 0;
    }
}

int main() {
    clear_screen();
    print_colored("Welcome to Activity Recommender CLI (ARC)\n", GREEN);
    print_colored("Loading embeddings and activities...\n", YELLOW);

    const char* embedding_file = "./data/glove.6B.300d.txt";

    // Check if the embeddings file exists
    FILE* test = fopen(embedding_file, "r");
    if (!test) {
        print_colored("Error: Could not open embeddings file\n", RED);
        exit(1);
    }
    fclose(test);

    // Create hash map to store word embeddings
    HashMap* embedding_map = create_hashmap();
    
    // Loading embeddings, activities, and calculate activity vectors
    load_embeddings(embedding_map, embedding_file);
    load_activities("./data/activities.csv");
    calculate_activity_vectors(embedding_map);

    print_colored("Ready! Type 'quit' to exit.\n", GREEN);

    char command[10];
    do {
        print_colored("\nEnter 'start' to begin or 'quit' to exit: ", BLUE);
        scanf("%s", command);
        getchar(); // Consume newline character left by scanf

        lowercase_str(command);  // Convert to lowercase
        if (strcmp(command, "start") == 0) {
            process_group_preferences(embedding_map);
        }
        else if (strcmp(command, "quit") != 0) {
            print_colored("Invalid command. Please try again.\n", RED);
        }
    } while (strcmp(command, "quit") != 0);  // Continue until input --> "quit"

    free_hashmap(embedding_map);
    cleanup_activities();
    print_colored("\nThank you for using ARC!\n", GREEN);

    return 0;
}