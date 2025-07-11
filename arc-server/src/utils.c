#include "../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

static ErrorCode last_error = ERROR_NONE;
static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

char* str_duplicate(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* dup = safe_malloc(len + 1);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

void str_to_lower(char* str) {
    if (!str) return;
    for (char* p = str; *p; p++) {
        *p = tolower(*p);
    }
}

void trim_whitespace(char* str) {
    if (!str) return;

    char* start = str;
    while (isspace(*start)) start++;
    
    if (*start == 0) {
        *str = 0;
        return;
    }

    char* end = str + strlen(str) - 1;
    while (end > start && isspace(*end)) end--;
    end[1] = 0;

    if (start > str) {
        memmove(str, start, end - start + 2);
    }
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        set_last_error(ERROR_MEMORY);
        return NULL;
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        set_last_error(ERROR_MEMORY);
        return NULL;
    }
    return new_ptr;
}

const char* get_error_message(ErrorCode code) {
    switch (code) {
        case ERROR_NONE:
            return "No error";
        case ERROR_MEMORY:
            return "Memory allocation failed";
        case ERROR_IO:
            return "Input/Output error";
        case ERROR_INVALID_INPUT:
            return "Invalid input";
        case ERROR_SERVER:
            return "Server error";
        default:
            return "Unknown error";
    }
}

void set_last_error(ErrorCode code) {
    last_error = code;
}

ErrorCode get_last_error(void) {
    return last_error;
}

void lock_init(void) {
    pthread_mutex_init(&global_lock, NULL);
}

void lock_acquire(void) {
    pthread_mutex_lock(&global_lock);
}

void lock_release(void) {
    pthread_mutex_unlock(&global_lock);
}

void lock_cleanup(void) {
    pthread_mutex_destroy(&global_lock);
}
