// Anything that starts with '#' is called a preprocessor directive

// START OF HEADER GUARD
// This checks if CUBE_H is not defined yet
// If CUBE_H is already defined, the preprocessor will skip everything until '#endif'

#ifndef CUBE_H

// Define CUBE_H so that next time this file is included, the check above will be true
// This prevents the contents from being included multiple times

#define CUBE_H

// The actual content of header file -->

int cube(int num);

// END OF HEADER GUARD
// This marks the end of the conditional block started by #ifndef

#endif
