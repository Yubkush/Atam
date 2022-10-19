#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr = (int*)malloc(sizeof(int) * 10);

    for (int i = 0; i < 10; i++)
    {
        arr[i] = i;
        printf("%d", arr[i]);
    }
    
    printf("\n");
    free(arr);
    return 0;
}