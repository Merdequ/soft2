#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(){
    time_t t;
    time(&t);
    printf("%ld", time(NULL));
}