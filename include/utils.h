#ifndef UTILS_H
#define UTILS_H

// Include standard C libraries that we need
#include <stdio.h>      // For printf, scanf
#include <stdlib.h>     // For memory allocation functions (malloc, free)
#include <string.h>     // For string manipulation functions (strcpy, strcmp)
#include <math.h>       // For mathematical functions (sqrt)
#include <ctype.h>      // For character handling functions (tolower)

// MACRO Definitions
#define MAX_WORD_LEN 100        // Maximum length of a word (100 characters)
#define MAX_LINE_LEN 8192       // Maximum length of a line in the file
#define EMBEDDING_DIM 300       // Dimension of our word vectors (300 for GloVe)
#define MAX_STOPWORDS 200       // Maximum number of stopwords that are stored
#define HASH_SIZE 100003        // Size of hash table (a prime number for better distribution)
#define MAX_TAGS 10             // Maximum number of tags per activity

// Structure to store a word and its embedding vector
typedef struct {
    char word[MAX_WORD_LEN];            // The word itself
    float vector[EMBEDDING_DIM];        // The 300-dimensional vector for this word
} WordEmbedding;

// Structure for a node in linked list (for hash table chaining)
typedef struct Node {
    WordEmbedding data;                 // The word embedding data
    struct Node* next;                  // Pointer to the next node in the list
} Node;

// Structure for hash map (to store and quickly look up word embeddings)
typedef struct {
    Node* table[HASH_SIZE];             // Array of pointers to linked lists
} HashMap;

// Structure to store an activity and its properties
typedef struct {
    char name[MAX_WORD_LEN];                    // Name of the activity
    char tags[MAX_TAGS][MAX_WORD_LEN];          // Tags describing the activity
    float avg_vector[EMBEDDING_DIM];            // Average vector of all tags
    int tag_count;                              // Number of tags for this activity
} Activity;

// List of common stopwords
static const char* stopwords[] = {
    "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
    "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
    "to", "was", "were", "will", "with", "the", "this", "but", "they",
    "have", "had", "what", "when", "where", "who", "which", "why", "how",
    NULL  // NULL --> End of Array
};

// Hash function to convert a word to a hash table index
unsigned int hash(const char* word);

// Create a new hash map
HashMap* create_hashmap(void);

// Insert a word and its vector into the hash map
void insert_embedding(HashMap* map, const char* word, float* vector);

// Find a word in the hash map and return its embedding
WordEmbedding* find_embedding(HashMap* map, const char* word);

// Free the memory used by the hash map
void free_hashmap(HashMap* map);

// Calculate the cosine similarity between two vectors
float cosine_similarity(float* vec1, float* vec2);

// Check if a word is a stopword
int is_stopword(const char* word);

// Convert a string to lowercase
void lowercase_str(char* str);

// Remove punctuation from a string
void remove_punctuation(char* str);

// Print colored text to the console
void print_colored(const char* text, const char* color);

// Clear the console screen
void clear_screen(void);

// Global variables for activities
Activity* activities;  // Pointer to array of activities
int activities_capacity;  // Current capacity of activities array
int activity_count;  // Number of activities loaded

#endif // UTILS_H