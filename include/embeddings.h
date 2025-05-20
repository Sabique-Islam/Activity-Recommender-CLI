#ifndef EMBEDDINGS_H
#define EMBEDDINGS_H

#include "common.h"
#include "colors.h"

typedef struct {
    char word[MAX_WORD_LEN];
    float vector[EMBEDDING_DIM];
} WordEmbedding;

typedef struct Node {
    WordEmbedding data;
    struct Node* next;
} Node;

typedef struct {
    Node* table[HASH_SIZE];
} HashMap;

unsigned int hash(const char* word);
HashMap* create_hashmap(void);
void insert_embedding(HashMap* map, const char* word, float* vector);
WordEmbedding* find_embedding(HashMap* map, const char* word);
void free_hashmap(HashMap* map);
float cosine_similarity(float* vec1, float* vec2);
void load_embeddings(HashMap* map, const char* filename);
void dump_hashmap_sample(HashMap* map);
void preprocess_tag(char* tag);

#endif