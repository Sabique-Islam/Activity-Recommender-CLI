#include <stdio.h>
#include "./header/cube.h"

int main(){
    int num = 5;
    int result = cube(num);
    printf("The cube of %d is %d\n", num, result);
    return 0;
}
