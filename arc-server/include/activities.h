#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include "common.h"
#include "embeddings.h"

typedef struct act {
    char name[MAX_WORD_LEN];
    char tags[MAX_TAGS][MAX_WORD_LEN];
    float avg_vector[EMBEDDING_DIM];
    int tag_count;
} Activity;

extern Activity* activities;
extern int activity_count;
extern int activities_capacity;

void load_activities(const char* filename);
void calculate_activity_vectors(HashMap* map);
void process_group_preferences(HashMap* map);
void cleanup_activities();

#endif