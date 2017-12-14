#include <stdio.h>

int factorial(int n){
    fprintf(stderr, "%d\n", n);
    
    /* if (n == 0) {
        return 1;
    }*/
    return n * factorial(n-1);
    
}

int main(void){
    int a = factorial(15);
    printf("\n ans \n %d \n", a);
}