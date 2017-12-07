/*
接近時にマージする処理を追加。
しようと思ったが間に合わず。
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 75
#define HEIGHT 40

const double G = 1.0;  // gravity constant
const double Threshold = 0.5;
struct star
{
  double m;   // mass
  double x;   // position_x
  double vx;  // velocity_x
  double y;
  double vy;
};

struct star stars[] = {
  { 1.0, -10.0, 0.0, 0.0, 0.0 },
  { 0.5,  10.0, 0.2, 0.0, 0.0 },
  { 0.7,  0.0, 0.0, 10.0, -0.3 }    
};

const int nstars = sizeof(stars) / sizeof(struct star);

void plot_stars(FILE *fp, const double t)
{
  int i;
  char space[WIDTH][HEIGHT];

  memset(space, ' ', sizeof(space));
  for (i = 0; i < nstars; i++) {
    const int x = WIDTH  / 2 + stars[i].x;
    const int y = HEIGHT / 2 + stars[i].y;
    if (x < 0 || x >= WIDTH)  continue;
    if (y < 0 || y >= HEIGHT) continue;
    char c = 'o';
    if (stars[i].m >= 1.0) c = 'O';
    space[x][y] = c;
  }

  int x, y;
  fprintf(fp, "----------\n");
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++)
      fputc(space[x][y], fp);
    fputc('\n', fp);
  }
  fflush(fp);

  printf("t = %5.1f", t);
  for (i = 0; i < nstars; i++){
    printf(", stars[%d].x = %7.2f", i, stars[i].x);
    printf(", stars[%d].y = %7.2f", i, stars[i].y);
  }    
  printf("\n");
}

void update_velocities(const double dt)
{
  int i, j;
  for (i = 0; i < nstars; i++) {
    double ax = 0;
    double ay = 0;
    

    /*  ???  */
    for (j = 0; j < nstars; j++) {
      if (i != j){ //違う星どうしなら引力が発生
        double d = sqrt(pow(stars[j].x - stars[i].x, 2) + \
        pow(stars[j].y - stars[i].y, 2));
        if (d < Threshold) { //ここに合体処理を書く
          ;
        }
        else {
        ax += G * stars[j].m * (stars[j].x - stars[i].x) \
        / pow(d, 3);
        ay += G * stars[j].m * (stars[j].y - stars[i].y) \
        / pow(d, 3);
        }
      }
    }    

    stars[i].vx += ax * dt;
    stars[i].vy += ay * dt;    
  }
}

void update_positions(const double dt)
{
  int i;
  for (i = 0; i < nstars; i++) {
    stars[i].x += stars[i].vx * dt;
    stars[i].y += stars[i].vy * dt;    
  }
}

int main(int argc, char **argv) {
  const char *filename = "space.txt";
  FILE *fp;
  if ((fp = fopen(filename, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return 1;
  }

  double inputdt = 0;
  if (argc == 2) {
    inputdt = atof(argv[1]);
  }
  const double dt = (inputdt != 0)? inputdt : 1.0;
  const double stop_time = 400;

  int i;
  double t;
  for (i = 0, t = 0; t <= stop_time; i++, t += dt) {
    update_velocities(dt);
    update_positions(dt);
    if (i % 10 == 0) {
      plot_stars(fp, t);
      usleep(200 * 1000);
    }
  }

  fclose(fp);

  return 0;
}
