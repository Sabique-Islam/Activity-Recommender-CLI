#include "../include/json_handler.h"
#include "../include/utils.h"
#include <string.h>

cJSON* parse_request_body(const char* body) {
    if (!body || strlen(body) == 0) {
        return NULL;
    }
    return cJSON_Parse(body);
}

char* create_error_response(const char* code, const char* message) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddFalseToObject(response, "success");
    
    cJSON* error = cJSON_CreateObject();
    cJSON_AddStringToObject(error, "code", code);
    cJSON_AddStringToObject(error, "message", message);
    cJSON_AddItemToObject(response, "error", error);


    char* response_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return response_str;
}

char* create_success_response(const char* message) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddTrueToObject(response, "success");
    cJSON_AddStringToObject(response, "message", message);


    char* response_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return response_str;
}

char* create_recommendations_response(const cJSON* recommendations) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddTrueToObject(response, "success");
    cJSON_AddItemToObject(response, "recommendations", cJSON_Duplicate(recommendations, 1));


    char* response_str = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    return response_str;
}