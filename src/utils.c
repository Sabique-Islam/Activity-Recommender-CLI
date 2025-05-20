#include "../include/utils.h"

int is_stopword(const char* word) {
    for (int i = 0; stopwords[i] != NULL; i++) {
        if (strcmp(word, stopwords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void lowercase_str(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void remove_punctuation(char* str) {
    int i, j = 0;

    for (i = 0; str[i]; i++) {
        if (!ispunct(str[i])) {
            str[j] = str[i];
            j++;
        }
    }

    str[j] = '\0';
}