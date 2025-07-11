#include "../include/embeddings.h"
#include "../include/utils.h"

unsigned int hash(const char* word) {
    unsigned int hash = 0;

    for (int i = 0; word[i] != '\0'; i++) {
        hash = 31 * hash + (unsigned char)word[i];
    }

    return hash % HASH_SIZE;
}

HashMap* create_hashmap(void) {
    HashMap* map = malloc(sizeof(HashMap));

    for (int i = 0; i < HASH_SIZE; i++) {
        map->table[i] = NULL;
    }

    return map;
}

void insert_embedding(HashMap* map, const char* word, float* vector) {
    unsigned int index = hash(word);

    Node* new_node = malloc(sizeof(Node));

    strncpy(new_node->data.word, word, MAX_WORD_LEN - 1);
    new_node->data.word[MAX_WORD_LEN - 1] = '\0';

    memcpy(new_node->data.vector, vector, EMBEDDING_DIM * sizeof(float));

    new_node->next = map->table[index];
    map->table[index] = new_node;
}

WordEmbedding* find_embedding(HashMap* map, const char* word) {
    char search_word[MAX_WORD_LEN];
    strncpy(search_word, word, MAX_WORD_LEN - 1);
    search_word[MAX_WORD_LEN - 1] = '\0';
    str_to_lower(search_word);

    int start = 0, end = strlen(search_word) - 1;

    memmove(search_word, search_word + start, end - start + 1);
    search_word[end - start + 1] = '\0';

    unsigned int index = hash(search_word);
    Node* current = map->table[index];

    while (current != NULL) {
        if (strcmp(current->data.word, search_word) == 0) {
            return &(current->data);
        }
        current = current->next;
    }

    return NULL;
}

void free_hashmap(HashMap* map) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Node* current = map->table[i];

        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
        }
    }

    free(map);
}

float cosine_similarity(float* vec1, float* vec2) {
    float dot_product = 0.0, mag1 = 0.0, mag2 = 0.0;

    for (int i = 0; i < EMBEDDING_DIM; i++) {
        mag1 += vec1[i] * vec1[i];
        mag2 += vec2[i] * vec2[i];
    }

    for (int i = 0; i < EMBEDDING_DIM; i++) {
        dot_product += vec1[i] * vec2[i];
    }

    float denominator = sqrt(mag1) * sqrt(mag2);
    float similarity = dot_product / denominator;

    return similarity;
}

void load_embeddings(HashMap* map, const char* filename) {
    FILE* file = fopen(filename, "r");
    char* line = malloc(MAX_LINE_LEN);
    char word[MAX_WORD_LEN];
    float vector[EMBEDDING_DIM];
    int count = 0;
    while (fgets(line, MAX_LINE_LEN, file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        char* space = strchr(line, ' ');
        size_t word_len = space - line;
        if (word_len >= MAX_WORD_LEN) {
            word_len = MAX_WORD_LEN - 1;
        }
        strncpy(word, line, word_len);
        word[word_len] = '\0';
        str_to_lower(word);

        char* pos = space + 1;
        int dim_count = 0;
        char* endptr;

        while (dim_count < EMBEDDING_DIM && *pos) {
            while (*pos && isspace(*pos)) pos++;
            vector[dim_count] = strtof(pos, &endptr);
            dim_count++;
            pos = endptr;
        }

        float magnitude = 0.0;
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            magnitude += vector[i] * vector[i];
        }
        magnitude = sqrt(magnitude);

        for (int i = 0; i < EMBEDDING_DIM; i++) {
            vector[i] /= magnitude;
        }

        insert_embedding(map, word, vector);
        count++;
        if (count % 50000 == 0) {
            printf(".");
            fflush(stdout);
        }
    }
    printf("\n----------Data loaded--------------\n");
    free(line);
    fclose(file);
}

void preprocess_tag(char* tag) {
    str_to_lower(tag);

    for (int i = 0; tag[i]; i++) {
        if (tag[i] == '-') {
            tag[i] = ' ';
        }
    }
}