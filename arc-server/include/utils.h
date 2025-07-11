#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

char* str_duplicate(const char* str);
void str_to_lower(char* str);
void trim_whitespace(char* str);

void* safe_malloc(size_t size);
void* safe_realloc(void* ptr, size_t size);

typedef enum {
    ERROR_NONE = 0,
    ERROR_MEMORY,
    ERROR_IO,
    ERROR_INVALID_INPUT,
    ERROR_SERVER
} ErrorCode;

const char* get_error_message(ErrorCode code);
void set_last_error(ErrorCode code);
ErrorCode get_last_error(void);

void lock_init(void);
void lock_acquire(void);
void lock_release(void);
void lock_cleanup(void);

#endif