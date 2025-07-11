#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include <stdbool.h>

int initialize_recommender(void);

typedef struct {
    char activity_name[100];
    float match_percentage;
    char tags[10][50];
    int tag_count;
} ActivityRecommendation;

typedef struct {
    char* keywords;
    int user_id;
} UserPreference;

ActivityRecommendation* get_group_recommendations(
    UserPreference* preferences,
    int group_size,
    int* num_recommendations
);

void cleanup_recommender(void);

#endif