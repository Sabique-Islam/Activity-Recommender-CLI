#include "../include/api.h"
#include "../include/json_handler.h"
#include "../include/recommender.h"
#include "../include/utils.h"
#include "../include/config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include </opt/homebrew/opt/libmicrohttpd/include/microhttpd.h>

struct RequestState {
    char *upload_data;
    size_t upload_data_size;
};

static enum MHD_Result send_response(struct MHD_Connection *connection,
                                   const char *response_str,
                                   unsigned int status_code) {
    if (!response_str) {
        response_str = "{}";
    }
    
    size_t response_len = strlen(response_str);
    struct MHD_Response *response = MHD_create_response_from_buffer(response_len,
                                                                  (void*)response_str,
                                                                  MHD_RESPMEM_MUST_COPY);
    if (!response) {
        return MHD_NO;
    }
    
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "http://localhost:3000");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
    MHD_add_response_header(response, "Content-Type", "application/json");
    
    char content_length[32];
    snprintf(content_length, sizeof(content_length), "%zu", response_len);
    MHD_add_response_header(response, "Content-Length", content_length);
    
    enum MHD_Result ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);

    return ret;
}

enum MHD_Result handle_recommend_endpoint(struct MHD_Connection *connection,
                                        const char *upload_data,
                                        size_t *upload_data_size,
                                        void **con_cls) {
    if (*con_cls == NULL) {
        struct RequestState *state = malloc(sizeof(struct RequestState));
        state->upload_data = NULL;
        state->upload_data_size = 0;
        *con_cls = state;
        return MHD_YES;
    }

    struct RequestState *state = *con_cls;

    if (*upload_data_size != 0) {
        if (state->upload_data == NULL) {
            state->upload_data = malloc(*upload_data_size + 1);
            memcpy(state->upload_data, upload_data, *upload_data_size);
            state->upload_data[*upload_data_size] = '\0';
            state->upload_data_size = *upload_data_size;
            *upload_data_size = 0;
            return MHD_YES;
        }
        return MHD_NO;
    }

    cJSON* request_json = parse_request_body(state->upload_data);
    
    if (!request_json) {
        const char *error = create_error_response("INVALID_JSON", "Invalid JSON in request body");
        enum MHD_Result ret = send_response(connection, error, MHD_HTTP_BAD_REQUEST);
        free((void*)error);

        if (state->upload_data) free(state->upload_data);
        free(state);
        *con_cls = NULL;
        return ret;
    }

    cJSON* group_size_json = cJSON_GetObjectItem(request_json, "groupSize");
    cJSON* preferences_json = cJSON_GetObjectItem(request_json, "preferences");

    if (!group_size_json || !preferences_json || !cJSON_IsNumber(group_size_json) || !cJSON_IsArray(preferences_json)) {
        const char *error = create_error_response("INVALID_REQUEST", "Invalid request format");
        enum MHD_Result ret = send_response(connection, error, MHD_HTTP_BAD_REQUEST);
        free((void*)error);

        cJSON_Delete(request_json);
        if (state->upload_data) free(state->upload_data);
        free(state);
        *con_cls = NULL;
        return ret;
    }

    int group_size = group_size_json->valueint;
    if (group_size <= 0 || group_size > MAX_GROUP_SIZE) {
        const char *error = create_error_response("INVALID_GROUP_SIZE", "Group size must be between 1 and MAX_GROUP_SIZE");
        enum MHD_Result ret = send_response(connection, error, MHD_HTTP_BAD_REQUEST);
        free((void*)error);

        cJSON_Delete(request_json);
        if (state->upload_data) free(state->upload_data);
        free(state);
        *con_cls = NULL;
        return ret;
    }

    UserPreference* preferences = safe_malloc(sizeof(UserPreference) * group_size);
    int num_preferences = 0;

    cJSON* preference;
    cJSON_ArrayForEach(preference, preferences_json) {
        if (num_preferences >= group_size) break;

        cJSON* keywords_json = cJSON_GetObjectItem(preference, "keywords");
        if (!keywords_json || !cJSON_IsArray(keywords_json)) continue;

        char keywords_str[1024] = "";
        cJSON* keyword;
        cJSON_ArrayForEach(keyword, keywords_json) {
            if (cJSON_IsString(keyword)) {
                if (keywords_str[0] != '\0') strcat(keywords_str, " ");
                strcat(keywords_str, keyword->valuestring);
            }
        }

        if (keywords_str[0] != '\0') {
            preferences[num_preferences].keywords = str_duplicate(keywords_str);
            preferences[num_preferences].user_id = num_preferences + 1;
            num_preferences++;
        }
    }

    if (num_preferences == 0) {
        const char *error = create_error_response("NO_VALID_PREFERENCES", "No valid preferences provided");
        enum MHD_Result ret = send_response(connection, error, MHD_HTTP_BAD_REQUEST);
        free((void*)error);

        free(preferences);
        cJSON_Delete(request_json);
        if (state->upload_data) free(state->upload_data);
        free(state);
        *con_cls = NULL;
        return ret;
    }

    int num_recommendations;
    ActivityRecommendation* recommendations = get_group_recommendations(preferences, num_preferences, &num_recommendations);

    const char *response_str;
    unsigned int status_code;

    if (!recommendations || num_recommendations == 0) {
        response_str = create_error_response("RECOMMENDATION_FAILED", "Failed to generate recommendations");
        status_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
    } else {
        cJSON* recommendations_json = cJSON_CreateArray();
        for (int i = 0; i < num_recommendations; i++) {
            cJSON* recommendation = cJSON_CreateObject();
            cJSON_AddStringToObject(recommendation, "activity", recommendations[i].activity_name);
            cJSON_AddNumberToObject(recommendation, "matchPercentage", recommendations[i].match_percentage);
            
            cJSON* tags = cJSON_CreateArray();
            for (int j = 0; j < recommendations[i].tag_count; j++) {
                cJSON_AddItemToArray(tags, cJSON_CreateString(recommendations[i].tags[j]));
            }
            cJSON_AddItemToObject(recommendation, "tags", tags);
            
            cJSON_AddItemToArray(recommendations_json, recommendation);
        }

        response_str = create_recommendations_response(recommendations_json);
        status_code = MHD_HTTP_OK;
        cJSON_Delete(recommendations_json);
    }

    enum MHD_Result ret = send_response(connection, response_str, status_code);
    free((void*)response_str);

    for (int i = 0; i < num_preferences; i++) {
        free(preferences[i].keywords);
    }
    free(preferences);
    free(recommendations);
    cJSON_Delete(request_json);
    if (state->upload_data) free(state->upload_data);
    free(state);
    *con_cls = NULL;

    return ret;
}