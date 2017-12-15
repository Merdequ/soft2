#include <stdio.h>



int main(void){
    int a[50];
    printf("%lu\n", sizeof(a));
    func(a);
}

int func(int array[50]){
    printf("%lu\n", sizeof(array));
}