#include "../include/colors.h"
#include <stdio.h>
#include <stdlib.h>

void print_colored(const char* text, const char* color) {
    printf("%s%s%s", color, text, "\033[0m");
}

void clear_screen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}