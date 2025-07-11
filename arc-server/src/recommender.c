#include "../include/recommender.h"
#include "../include/utils.h"
#include "../include/config.h"
#include "../include/embeddings.h"
#include "../include/activities.h"
#include "../include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static HashMap* embedding_map = NULL;

int initialize_recommender(void) {
    pthread_mutex_lock(&init_mutex);

    embedding_map = create_hashmap();
    if (!embedding_map) {
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    const char* embedding_file = "./data/glove.6B.300d.txt";
    FILE* test_file = fopen(embedding_file, "r");
    if (!test_file) {
        free_hashmap(embedding_map);
        embedding_map = NULL;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }
    fclose(test_file);

    load_embeddings(embedding_map, embedding_file);
    const char* activities_file = "./data/activities.csv";
    test_file = fopen(activities_file, "r");
    if (!test_file) {
        free_hashmap(embedding_map);
        embedding_map = NULL;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }
    fclose(test_file);

    load_activities(activities_file);
    calculate_activity_vectors(embedding_map);

    pthread_mutex_unlock(&init_mutex);
    return 0;
}

ActivityRecommendation* get_group_recommendations(
    UserPreference* preferences,
    int group_size,
    int* num_recommendations
) {
    if (!preferences || group_size <= 0) {
        *num_recommendations = 0;
        return NULL;
    }
    float group_vector[EMBEDDING_DIM] = {0};
    int total_valid_words = 0;
    for (int i = 0; i < group_size; i++) {
        char* word = strtok(preferences[i].keywords, " ");
        while (word) {
            if (strlen(word) > 0) {
                WordEmbedding* embedding = find_embedding(embedding_map, word);
                if (embedding) {
                    for (int j = 0; j < EMBEDDING_DIM; j++) {
                        group_vector[j] += embedding->vector[j];
                    }
                    total_valid_words++;
                } else {
                    char word_copy[MAX_WORD_LEN];
                    strncpy(word_copy, word, MAX_WORD_LEN - 1);
                    word_copy[MAX_WORD_LEN - 1] = '\0';

                    for (int k = 0; word_copy[k]; k++) {
                        if (word_copy[k] == '-') word_copy[k] = ' ';
                    }

                    char* subword = strtok(word_copy, " ");
                    int subword_count = 0;
                    float temp_vector[EMBEDDING_DIM] = {0};

                    while (subword) {
                        WordEmbedding* sub_embedding = find_embedding(embedding_map, subword);
                        if (sub_embedding) {
                            for (int j = 0; j < EMBEDDING_DIM; j++) {
                                temp_vector[j] += sub_embedding->vector[j];
                            }
                            subword_count++;
                        }
                        subword = strtok(NULL, " ");
                    }

                    if (subword_count > 0) {
                        for (int j = 0; j < EMBEDDING_DIM; j++) {
                            temp_vector[j] /= subword_count;
                            group_vector[j] += temp_vector[j];
                        }
                        total_valid_words++;
                    }
                }
            }
            word = strtok(NULL, " ");
        }
    }

    if (total_valid_words == 0) {
        *num_recommendations = 0;
        return NULL;
    }
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        group_vector[i] /= total_valid_words;
    }
    ActivityRecommendation* recommendations = safe_malloc(3 * sizeof(ActivityRecommendation));
    if (!recommendations) {
        *num_recommendations = 0;
        return NULL;
    }
    for (int i = 0; i < 3; i++) {
        recommendations[i].match_percentage = -1;
    }

    typedef struct {
        int index;
        float similarity;
    } ActivityMatch;
    ActivityMatch* matches = safe_malloc(activity_count * sizeof(ActivityMatch));
    for (int i = 0; i < activity_count; i++) {
        matches[i].index = i;
        matches[i].similarity = cosine_similarity(group_vector, activities[i].avg_vector);
    }
    for (int i = 0; i < activity_count - 1; i++) {
        for (int j = i + 1; j < activity_count; j++) {
            if (matches[j].similarity > matches[i].similarity) {
                ActivityMatch tmp = matches[i];
                matches[i] = matches[j];
                matches[j] = tmp;
            }
        }
    }
    int num_to_return = activity_count < 3 ? activity_count : 3;
    for (int i = 0; i < num_to_return; i++) {
        int idx = matches[i].index;
        strncpy(recommendations[i].activity_name, activities[idx].name, sizeof(recommendations[i].activity_name) - 1);
        recommendations[i].match_percentage = matches[i].similarity * 100;
        recommendations[i].tag_count = activities[idx].tag_count;
        for (int t = 0; t < activities[idx].tag_count && t < 10; t++) {
            strncpy(recommendations[i].tags[t], activities[idx].tags[t], sizeof(recommendations[i].tags[t]) - 1);
        }
    }
    free(matches);
    *num_recommendations = num_to_return;
    return recommendations;
}

void cleanup_recommender(void) {
    pthread_mutex_lock(&init_mutex);
    if (embedding_map) {
        free_hashmap(embedding_map);
        embedding_map = NULL;
    }
    cleanup_activities();
    pthread_mutex_unlock(&init_mutex);
}