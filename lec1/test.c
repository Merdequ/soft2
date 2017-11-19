#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(){
printf("atamagaokasiiiiiiiiiiiii\n");

  struct timespec req = {0, 100 * 1000000};
  for (int i = 0; i < 100; i++){
  	printf("%d\n", i);
  	nanosleep(&req, NULL);
	}
}
