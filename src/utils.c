#include "../include/utils.h"

// Hash function to convert a word to a hash table index
unsigned int hash(const char* word) {
    unsigned int hash = 0;  // Start with a hash value of 0
    
    // For each character in the word
    for (int i = 0; word[i] != '\0'; i++) {
        // Multiply the current hash by 31 (a prime number) and add the character ( Common string hashing algorithm )
        hash = 31 * hash + (unsigned char)word[i];
    }
    
    // Makes sure the result is within the bounds of our hash table
    return hash % HASH_SIZE;
}

// Create a new hash map
HashMap* create_hashmap(void) {
    // Allocate memory for the hash map structure
    HashMap* map = malloc(sizeof(HashMap));
    
    // Initialize all table entries to NULL (empty)
    for (int i = 0; i < HASH_SIZE; i++) {
        map->table[i] = NULL;
    }
    
    return map;  // Return the newly created hash map
}

// Insert a word and its vector into the hash map
void insert_embedding(HashMap* map, const char* word, float* vector) {
    // Calculate the hash index for this word
    unsigned int index = hash(word);

    // Create a new node for this word and its vector
    Node* new_node = malloc(sizeof(Node));
    
    // Copy the word into the node (with bounds checking)
    strncpy(new_node->data.word, word, MAX_WORD_LEN - 1);
    new_node->data.word[MAX_WORD_LEN - 1] = '\0';  // Ensure null termination
    
    // Copy the vector into the node
    memcpy(new_node->data.vector, vector, EMBEDDING_DIM * sizeof(float));

    // Insert the node at the beginning of the linked list for this hash index
    // This is called "chaining" - a way to handle hash collisions
    new_node->next = map->table[index];
    map->table[index] = new_node;
}

// Find a word in the hash map and return its embedding

WordEmbedding* find_embedding(HashMap* map, const char* word) {
    // Check for NULL inputs

    if (!word || !map) return NULL;

    // Create a lowercase copy of the search word
    char search_word[MAX_WORD_LEN];
    strncpy(search_word, word, MAX_WORD_LEN - 1);

    search_word[MAX_WORD_LEN - 1] = '\0';  // Ensure null termination
    lowercase_str(search_word);  // Convert to lowercase for case-insensitive search

    // Trim any leading/trailing whitespace
    int start = 0, end = strlen(search_word) - 1;
    
    // Find the first non-whitespace character
    while (isspace(search_word[start])) 
        start++;
    
    // Find the last non-whitespace character
    while (end >= start && isspace(search_word[end])) 
        end--;

    // If the string is all whitespace, return NULL
    if (start>end) 
        return NULL;

    // Move the trimmed string to the beginning
    if (start>0) {
        memmove(search_word, search_word + start, end - start + 1);
        search_word[end - start + 1] = '\0';
    } else {
        search_word[end + 1] = '\0';
    }

    // Calculate the hash index for the search word
    unsigned int index = hash(search_word);
    
    // Start at the first node in the linked list for this hash index
    Node* current = map->table[index];

    // Traverse the linked list, looking for a match
    while (current != NULL) {

        // Compare with lowercase version
        if (strcmp(current->data.word, search_word) == 0) {

            // Found a match! Return a pointer to the embedding
            return &(current->data);
        }

        current = current->next;  // Move to next node
    }

    // Word wasn't found
    return NULL;
}

// Free memory used by hash map
void free_hashmap(HashMap* map) {

    // For each bucket in the hash table
    for (int i = 0; i < HASH_SIZE; i++) {

        // Start at the first node in this bucket
        Node* current = map->table[i];
        
        // Free all nodes in the linked list

        while (current != NULL) {
            Node* next = current->next;  // Remember the next node
            free(current);  // Free the current node
            current = next;  // Move to the next node
        }
    }
    
    free(map);
}

// Calculate the cosine similarity between two vectors
float cosine_similarity(float* vec1, float* vec2) {
    float dot_product = 0.0, mag1 = 0.0, mag2 = 0.0;

    // First pass: calculate magnitudes and check for NaN/Inf
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        // Check for invalid values (NaN or infinity)
        if (isnan(vec1[i]) || isnan(vec2[i]) ||
            isinf(vec1[i]) || isinf(vec2[i])) {
            return 0.0;  // Return 0 for invalid vectors
        }
        
        // Calculate the squares for the magnitudes
        mag1 += vec1[i] * vec1[i];
        mag2 += vec2[i] * vec2[i];
    }

    // Check if either magnitude is too small (to avoid division by zero)
    if (mag1 < 1e-10 || mag2 < 1e-10) {
        return 0.0;  // Return 0 for zero vectors
    }

    // Calculate dot product
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        dot_product += vec1[i] * vec2[i];
    }

    // Calculate the denominator (product of magnitudes)
    float denominator = sqrt(mag1) * sqrt(mag2);
    
    // Calculate the final similarity
    float similarity = dot_product / denominator;

    return similarity;  // Return the cosine similarity (between -1 and 1)
}

// Check if a word is a stopword
int is_stopword(const char* word) {

    // Check each stopword in our list
    for (int i = 0; stopwords[i] != NULL; i++) {

        if (strcmp(word, stopwords[i]) == 0) {
            return 1;  // Stopword
        }
    }
    return 0;  // Not Stopword
}

// Convert a string to lowercase
void lowercase_str(char* str) {

    // For each character in the string
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);  // Convert to lowercase
    }
}

void remove_punctuation(char* str) {
    int i, j = 0;
    
    for (i = 0; str[i]; i++) {

        // If it's not punctuation
        if (!ispunct(str[i])) {

            str[j] = str[i];  // Keep this character
            j++;  // Move to the next position in the output
        }

        // Skip all punctuation
    }
    
    str[j] = '\0';  // Add null terminator at the new end of the string
}

// Print colored text to the console
void print_colored(const char* text, const char* color) {
    // Print the color code, then the text, then reset the color
    printf("%s%s%s", color, text, "\033[0m");
}

// Clear the console screen
void clear_screen(void) {
    #ifdef _WIN32
        system("cls"); // For Windows
    #else
        system("clear"); // For Unix-based systems (MacOS && Linux)
    #endif
}
