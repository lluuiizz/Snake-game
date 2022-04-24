#include <stdio.h>

void recursion(int , int, int i);



int main(void) {
    recursion (0, 10, 0);
    
    return 0;
}

void recursion (int x, int limit, int i) {
    if (i < limit) {
        x = x + 1;
        i = i+1;
    }
    printf("i = %d\t\tx = %d\n", i, x);
    if (i != limit) {
        recursion(x, limit, i);
    
    
    }
}
