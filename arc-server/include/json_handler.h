#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include "../cjson/cJSON.h"

cJSON* parse_request_body(const char* body);

char* create_error_response(const char* code, const char* message);
char* create_success_response(const char* message);
char* create_recommendations_response(const cJSON* recommendations);

#endif
