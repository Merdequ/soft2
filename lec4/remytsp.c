#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <float.h>

#define WIDTH 70
#define HEIGHT 40
#define MAX_CITIES 1000

typedef struct
{
  int x;
  int y;
} City;

typedef struct
{
  double distance;
  int *route;
} Routedata;

City city[MAX_CITIES];
char map[WIDTH][HEIGHT];

double **dist_data;

int max(const int a, const int b)
{
  return (a > b) ? a : b;
}

void draw_line(const int x0, const int y0, const int x1, const int y1)
{
  int i;
  const int n = max(abs(x1 - x0), abs(y1 - y0)) + 1;
  for (i = 0; i <= n; i++) {
    const int x = x0 + i * (x1 - x0) / n;
    const int y = y0 + i * (y1 - y0) / n;
    if (map[x][y] == ' ') map[x][y] = '*';
  }
}

void draw_route(const int n, const int *route)
{
  if (route == NULL) return;

  int i;
  for (i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = (i < n - 1) ? route[i+1] : route[0];
    draw_line(city[c0].x, city[c0].y, city[c1].x, city[c1].y);
  }
}

void plot_cities(FILE *fp, const int n, const int *route)
{
  int i, j, x, y;

  fprintf(fp, "----------\n");

  memset(map, ' ', sizeof(map));

  for (i = 0; i < n; i++) {
    char buf[100];
    sprintf(buf, "C_%d", i);
    for (j = 0; j < strlen(buf); j++) {
      const int x = city[i].x + j;
      const int y = city[i].y;
      map[x][y] = buf[j];
    }
  }

  draw_route(n, route);
    
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      const char c = map[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }

  fflush(fp);
}

double distance(const int i, const int j)
{
  const double dx = city[i].x - city[j].x;
  const double dy = city[i].y - city[j].y;
  return sqrt(dx * dx + dy * dy);
}

double route_distance(const int n, int route[]){
  // Compute the total distance
  double sum_d = 0;
  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n]; 
    sum_d += distance(c0, c1);
  }
  return sum_d;
}

Routedata *search(const int n, int i, int route[]){
  // fprintf(stderr, "visiting city %d at (%d, %d)\n", route[i], city[route[i]].x, city[route[i]].y);
  if (i == n - 1){
    // return distance(route[i], route[0]);
    Routedata *bestroute = (Routedata *)malloc(sizeof(Routedata));
    bestroute->route = (int *)malloc(sizeof(int) * n);
    memcpy(bestroute->route, route, sizeof(int) * n);
    bestroute->distance = dist_data[ route[i] ][ route[0] ];
    return bestroute;
  }
  Routedata *temp;
  Routedata *bestroute = (Routedata *)malloc(sizeof(Routedata));
  bestroute->route = NULL;
  bestroute->distance = DBL_MAX;
  for (int next = 1; next < n; next++){
    int is_valid = 1;
    for (int k = 1; k <= i; k++){
      if (route[k] == next){
        is_valid = 0;
      }
    }
    if (!is_valid){
      continue;
    }
    route[i+1] = next;
    temp = search(n, i + 1, route);
    temp->distance += dist_data[ route[i] ][ route[(i+1)%n] ];
    if(temp->distance < bestroute->distance){
      if (bestroute->route != NULL){
        free(bestroute->route);
      }
      free(bestroute);
      bestroute = temp;
    }
    else{
      free(temp->route);
      free(temp);
    }
  }
  return bestroute;
}

double solve(const int n, int route[]) // routeは結果をmainに返す用の配列
{
  int temproute[sizeof(int) * n];
  temproute[0] = 0;  // Start from city[0]

  Routedata *bestroute = search(n, 0, temproute);
  memcpy(route, bestroute->route, sizeof(int) * n);
  double dist = bestroute->distance;
  free(bestroute->route);
  free(bestroute);
  return dist;
}

int main(const int argc, const char **argv)
{
  FILE *fp;
  const char *map_file = "map.txt";
  if ((fp = fopen(map_file, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", map_file);
    return 1;
  }

  if (argc != 2) {
    fprintf(stderr, "error: please specify the number of cities.\n");
    exit(1);
  }

  const int n = atoi(argv[1]);
  assert(n > 1 && n <= MAX_CITIES);

  int i;
  for (i = 0; i < n; i++) {
    city[i].x = rand() % (WIDTH - 5);
    city[i].y = rand() % HEIGHT;
  }

  plot_cities(fp, n, NULL);
  sleep(1);

  // caliculate distance
  dist_data = (double **)malloc(n * sizeof(double *));
  for (int i=0; i < n; i++){
    dist_data[i] = (double *)malloc(n * sizeof(double));
  }
  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      dist_data[i][j] = distance(i, j);
    }
  }

  int route[MAX_CITIES];
  const double d = solve(n, route);

  printf("total distance = %f\n", d);
  plot_cities(fp, n, route);

  return 0;
}
