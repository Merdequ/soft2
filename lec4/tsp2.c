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

#define R_DEFAULT 0.999995

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

double **distance_matrix;

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

double route_distance(const int n, const int route[]){
  // Compute the total distance
  double sum_d = 0;
  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n]; 
    sum_d += distance(c0, c1);
  }
  return sum_d;
}

double route_distance_quick(const int n, const int route[]){
  // Compute the total distance
  double sum_d = 0;
  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n]; 
    sum_d += distance_matrix[c0][c1];
  }
  return sum_d;
}

void swap(int array[], int i, int j){
  int temp = array[i];
  array[i] = array[j];
  array[j] = temp;
}

void random_neighbor(const int n, const int current[], Routedata *neighbor){
  assert(n > 2);
  memcpy(neighbor->route, current, n * sizeof(int));
  int i = 1 + rand() % (n - 1);
  int j;
  while ((j = 1 + rand() % (n - 1)) == i) ;
  swap(neighbor->route, i, j);
  neighbor->distance = route_distance_quick(n, neighbor->route);
}

Routedata yakinamashi(const int n, const int init_route[], const double r){
  Routedata current_route;
  current_route.route = (int *)malloc(n * sizeof(int));
  memcpy(current_route.route, init_route, n * sizeof(int));
  current_route.distance = route_distance_quick(n, init_route);

  if(n <= 3){
    return current_route;
  }

  double T = 1000;
  const double T_threshhold = 0.001;

  Routedata temp_route;
  temp_route.route = (int *)malloc(n * sizeof(int));

  while(T > T_threshhold){
    random_neighbor(n, current_route.route, &temp_route);
    double d = - temp_route.distance + current_route.distance;
    if ( exp(d / T) > rand() / RAND_MAX){
      memcpy(current_route.route, temp_route.route, n * sizeof(int));
      current_route.distance = temp_route.distance;
      //fprintf(stderr, "%lf %lf %lf\n", T, d, current_route.distance);
    }
    T *= r;
  }
  free(temp_route.route);
  return current_route;
}

double solve(const int n, int route[], const double r) // routeはメイン関数に結果を返す用
{
  int init_route[n];
  for (int i = 0; i < n; i++){
    init_route[i] = i;
  }

  Routedata final_route = yakinamashi(n, init_route, r);

  double sum_d = final_route.distance;
  memcpy(route, final_route.route, n * sizeof(int));
  free(final_route.route);
  return sum_d;
}

int main(const int argc, const char **argv)
{
  FILE *fp;
  const char *map_file = "map.txt";
  if ((fp = fopen(map_file, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", map_file);
    return 1;
  }

  if (argc < 2) {
    fprintf(stderr, "error: please specify the number of cities.\n");
    exit(1);
  }

  const int n = atoi(argv[1]);
  assert(n > 1 && n <= MAX_CITIES);

  double r;
  if (argc == 3){
    r = atof(argv[2]);
    assert(r > 0 && r < 1);
  }
  else{
    r = R_DEFAULT ;
    printf("r set to default value: %lf\n", R_DEFAULT);
  }

  int seed = 0;
  srand(seed);

  int i;
  for (i = 0; i < n; i++) {
    city[i].x = rand() % (WIDTH - 5);
    city[i].y = rand() % HEIGHT;
  }

  // make matrix of distance
  distance_matrix = (double **)malloc(n * sizeof(double *));
  for (int i=0; i < n; i++){
    distance_matrix[i] = (double *)malloc(n * sizeof(double));
  }
  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      distance_matrix[i][j] = distance(i, j);
    }
  }

  plot_cities(fp, n, NULL);
  sleep(1);

  int route[MAX_CITIES];
  const double d = solve(n, route, r);
  printf("total distance = %f\n", d);  
  plot_cities(fp, n, route);
  fprintf(fp, "total distance = %f\n", d);  
  return 0;
}