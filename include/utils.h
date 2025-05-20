#ifndef UTILS_H
#define UTILS_H

#include "common.h"

static const char* stopwords[] = {
    "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
    "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
    "to", "was", "were", "will", "with", "the", "this", "but", "they",
    "have", "had", "what", "when", "where", "who", "which", "why", "how",
    NULL
};

int is_stopword(const char* word);
void lowercase_str(char* str);
void remove_punctuation(char* str);

#endif