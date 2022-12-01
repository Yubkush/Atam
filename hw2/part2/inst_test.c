#include <stdio.h>
 
int main()
{
    unsigned char x = 1;
    printf("hello %hhx\n", x);
    asm(".int 0x90040f90":"=a"(x):"a"(x));
    printf("hello %hhx\n", x);
    asm(".int 0x90ff0f90":"=a"(x):"a"(x));
    printf("hello %hhx\n", x);
    return 0; 
}
