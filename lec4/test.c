#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <float.h>


void swap(int array[], int i, int j){
  int temp = array[i];
  array[i] = array[j];
  array[j] = temp;
}


void random_arrange(const int n, int array[]){
  for (int i = 0; i < n; i++) {
    array[i] = i;
  }
  // Shuffle
  for (int i = 1; i < n; i++){
    int j = 1 + rand() % (n - 1);
    swap(array, i, j);
  }
}

int main(void){
    int seed = 0;
    srand(seed);
    int a[5];
    int n = (sizeof(a)/sizeof(int));
    for (int i = 0; i < 12; i++){
        random_arrange(n, a);
        for (int j = 0; j < n; j++){
            printf("%d ", a[j]);
        }
        printf("\n");
    }
}
