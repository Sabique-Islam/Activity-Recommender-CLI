#ifndef COLORS_H
#define COLORS_H

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

void print_colored(const char* text, const char* color);
void clear_screen(void);

#endif