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
    if (!word || !map) return NULL;

    char search_word[MAX_WORD_LEN];
    strncpy(search_word, word, MAX_WORD_LEN - 1);

    search_word[MAX_WORD_LEN - 1] = '\0';
    lowercase_str(search_word);

    int start = 0, end = strlen(search_word) - 1;

    while (isspace(search_word[start]))
        start++;

    while (end >= start && isspace(search_word[end]))
        end--;

    if (start > end)
        return NULL;

    if (start > 0) {
        memmove(search_word, search_word + start, end - start + 1);
        search_word[end - start + 1] = '\0';
    } else {
        search_word[end + 1] = '\0';
    }

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
        if (isnan(vec1[i]) || isnan(vec2[i]) ||
            isinf(vec1[i]) || isinf(vec2[i])) {
            return 0.0;
        }

        mag1 += vec1[i] * vec1[i];
        mag2 += vec2[i] * vec2[i];
    }

    if (mag1 < 1e-10 || mag2 < 1e-10) {
        return 0.0;
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
    if (!file) {
        print_colored("\nError: Could not open embeddings file: ", RED);
        printf("%s\n", filename);
        exit(1);
    }
    print_colored("Loading embeddings... ", YELLOW);

    char* line = malloc(MAX_LINE_LEN);
    if (!line) {
        print_colored("Error: Memory allocation failed\n", RED);
        fclose(file);
        exit(1);
    }

    char word[MAX_WORD_LEN];
    float vector[EMBEDDING_DIM];
    int count = 0;

    while (fgets(line, MAX_LINE_LEN, file)) {

        size_t len = strlen(line);
        if (len == MAX_LINE_LEN - 1 && line[len-1] != '\n') {
            int c;
            while ((c = fgetc(file)) != EOF && c != '\n');
            continue;
        }

        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        if (len == 0){
             continue;
        }

        char* space = strchr(line, ' ');
        if (!space) continue;

        size_t word_len = space - line;
        if (word_len >= MAX_WORD_LEN) {
            word_len = MAX_WORD_LEN - 1;
        }
        strncpy(word, line, word_len);
        word[word_len] = '\0';
        lowercase_str(word);

        char* pos = space + 1;
        int dim_count = 0;
        char* endptr;

        while (dim_count < EMBEDDING_DIM && *pos) {
            while (*pos && isspace(*pos)) pos++;
            if (!*pos)
                break;

            vector[dim_count] = strtof(pos, &endptr);

            if (pos == endptr)
                break;

            dim_count++;
            pos = endptr;
        }

        if (dim_count != EMBEDDING_DIM) continue;

        float magnitude = 0.0;
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            magnitude += vector[i] * vector[i];
        }
        magnitude = sqrt(magnitude);

        if (magnitude > 1e-10) {
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
    }

    free(line);
    printf("\nLoaded %d words from embeddings file.\n", count);
    fclose(file);
}

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

void preprocess_tag(char* tag) {
    lowercase_str(tag);

    for (int i = 0; tag[i]; i++) {
        if (tag[i] == '-') {
            tag[i] = ' ';
        }
    }
}