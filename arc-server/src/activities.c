#include "../include/activities.h"
#include "../include/utils.h"

Activity* activities = NULL;
int activity_count = 0;
int activities_capacity = 0;

void load_activities(const char* filename) {
    FILE* file = fopen(filename, "r");
    activities_capacity = INITIAL_CAPACITY;
    activities = malloc(activities_capacity * sizeof(Activity));
    activity_count = 0;

    char line[MAX_LINE_LEN];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (activity_count >= activities_capacity) {
            activities_capacity *= GROWTH_FACTOR;
            Activity* temp = realloc(activities, activities_capacity * sizeof(Activity));
            activities = temp;
        }

        char* token = strtok(line, ",");
        strncpy(activities[activity_count].name, token, MAX_WORD_LEN - 1);
        activities[activity_count].name[MAX_WORD_LEN - 1] = '\0';
        activities[activity_count].tag_count = 0;

        while ((token = strtok(NULL, ",\n")) && activities[activity_count].tag_count < MAX_TAGS) {
            char lowercase_token[MAX_WORD_LEN];
            strncpy(lowercase_token, token, MAX_WORD_LEN - 1);
            lowercase_token[MAX_WORD_LEN - 1] = '\0';
            str_to_lower(lowercase_token);

            strncpy(activities[activity_count].tags[activities[activity_count].tag_count],
                   lowercase_token, MAX_WORD_LEN - 1);
            activities[activity_count].tag_count++;
        }

        activity_count++;
    }

    if (activity_count < activities_capacity) {
        Activity* temp = realloc(activities, activity_count * sizeof(Activity));
        if (temp) {
            activities = temp;
            activities_capacity = activity_count;
        }
    }
    fclose(file);
}

void calculate_activity_vectors(HashMap* map) {
    for (int i = 0; i < activity_count; i++) {
        memset(activities[i].avg_vector, 0, sizeof(float) * EMBEDDING_DIM);
        int valid_tags = 0;

        for (int j = 0; j < activities[i].tag_count; j++) {
            WordEmbedding* embedding = find_embedding(map, activities[i].tags[j]);
            if (embedding) {
                for (int k = 0; k < EMBEDDING_DIM; k++) {
                    activities[i].avg_vector[k] += embedding->vector[k];
                }
                valid_tags++;
            } else {
                char tag_copy[MAX_WORD_LEN];
                strncpy(tag_copy, activities[i].tags[j], MAX_WORD_LEN - 1);
                tag_copy[MAX_WORD_LEN - 1] = '\0';

                for (int k = 0; tag_copy[k]; k++) {
                    if (tag_copy[k] == '-') {
                        tag_copy[k] = ' ';
                    }
                }

                char* word;
                int word_count = 0;
                float temp_vector[EMBEDDING_DIM] = {0};

                word = strtok(tag_copy, " ");

                while (word) {
                    if (strlen(word) > 0) {
                        WordEmbedding* sub_embedding = find_embedding(map, word);
                        if (sub_embedding) {
                            for (int k = 0; k < EMBEDDING_DIM; k++) {
                                temp_vector[k] += sub_embedding->vector[k];
                            }
                            word_count++;
                        }
                    }
                    word = strtok(NULL, " ");
                }

                if (word_count > 0) {
                    for (int k = 0; k < EMBEDDING_DIM; k++) {
                        temp_vector[k] /= word_count;
                        activities[i].avg_vector[k] += temp_vector[k];
                    }
                    valid_tags++;
                }
            }
        }

        if (valid_tags > 0) {
            for (int k = 0; k < EMBEDDING_DIM; k++) {
                activities[i].avg_vector[k] /= valid_tags;
            }
        } else {
            for (int k = 0; k < EMBEDDING_DIM; k++) {
                activities[i].avg_vector[k] = 1e-6;
            }
        }
    }
}

void process_group_preferences(HashMap* map) {
    int group_size;
    scanf("%d", &group_size);
    getchar();

    float group_vector[EMBEDDING_DIM] = {0};
    int total_valid_words = 0;

    for (int i = 0; i < group_size; i++) {
        char preferences[MAX_LINE_LEN];
        fgets(preferences, sizeof(preferences), stdin);
        preferences[strcspn(preferences, "\n")] = 0;
        
        str_to_lower(preferences);
        trim_whitespace(preferences);

        char* word = strtok(preferences, " ");
        while (word) {
            if (strlen(word) > 0) {
                WordEmbedding* embedding = find_embedding(map, word);
                if (embedding) {
                    for (int j = 0; j < EMBEDDING_DIM; j++) {
                        group_vector[j] += embedding->vector[j];
                    }
                    total_valid_words++;
                } else {
                    char word_copy[MAX_WORD_LEN];
                    strncpy(word_copy, word, MAX_WORD_LEN - 1);
                    word_copy[MAX_WORD_LEN - 1] = '\0';

                    int has_hyphen = 0;
                    for (int k = 0; word_copy[k]; k++) {
                        if (word_copy[k] == '-') {
                            word_copy[k] = ' ';
                            has_hyphen = 1;
                        }
                    }

                    if (has_hyphen) {
                        char* subword;
                        int subword_count = 0;
                        float temp_vector[EMBEDDING_DIM] = {0};

                        subword = strtok(word_copy, " ");
                        while (subword) {
                            if (strlen(subword) > 0) {
                                WordEmbedding* sub_embedding = find_embedding(map, subword);
                                if (sub_embedding) {
                                    for (int j = 0; j < EMBEDDING_DIM; j++) {
                                        temp_vector[j] += sub_embedding->vector[j];
                                    }
                                    subword_count++;
                                }
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
            }
            word = strtok(NULL, " ");
        }
    }

    if (total_valid_words > 0) {
        for (int i = 0; i < EMBEDDING_DIM; i++) {
            group_vector[i] /= total_valid_words;
        }

        typedef struct {
            int index;
            float similarity;
        } ActivityMatch;

        ActivityMatch top3[3] = {{-1, -1}, {-1, -1}, {-1, -1}};

        for (int i = 0; i < activity_count; i++) {
            float sim = cosine_similarity(group_vector, activities[i].avg_vector);
            for (int j = 0; j < 3; j++) {
                if (sim > top3[j].similarity) {
                    for (int k = 2; k > j; k--) {
                        top3[k] = top3[k-1];
                    }
                    top3[j].index = i;
                    top3[j].similarity = sim;
                    break;
                }
            }
        }
    }
}

void cleanup_activities() {
    if (activities) {
        free(activities);
        activities = NULL;
        activity_count = 0;
        activities_capacity = 0;
    }
}