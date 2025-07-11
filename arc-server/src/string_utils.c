#include "../include/utils.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void str_to_lower(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void trim_whitespace(char* str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if (!ispunct(str[i])) {
            str[j] = str[i];
            j++;
        }
    }
    str[j] = '\0';
}

static const char* stopwords[] = {
    "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
    "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
    "to", "was", "were", "will", "with", "the", "this", "but", "they",
    "have", "had", "what", "when", "where", "who", "which", "why", "how",
    NULL
};

bool is_stopword(const char* word) {
    for (int i = 0; stopwords[i] != NULL; i++) {
        if (strcmp(word, stopwords[i]) == 0) {
            return true;
        }
    }
    return false;
}
