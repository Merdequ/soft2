/*
コマンドライン入力によるペイントソフト
コマンド履歴をリストに保存する

既存コマンド:
line x0 y0 x1 y1  --- (x0,y0), (x1,y1)を結ぶ直線を描画
undo  --- 最後に入力したコマンドを取り消す
save [filename] --- コマンド履歴を指定したファイルに保存(未指定の場合デフォルトのファイル)
quit  --- プログラムを終了
追加コマンド:
rectangle x0 y0 width height  --- 左上の頂点が(x0, y0)で指定した幅と高さの長方形を描画
circle x0 y0 r  --- 中心が (x0, y0) で半径が r の円を描画
load [filename] --- 指定したファイル(未指定の場合デフォルトのファイル)から読み込み
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#define WIDTH 70
#define HEIGHT 40

#define BUFSIZE 1000

int interpret_command(const char *command);

// List structure
struct node 
{
  char *str;
  struct node *next;
};

typedef struct node Node;

Node *push_front(Node *begin, const char *str)
{
  // Create a new element
  Node *p = (Node *)malloc(sizeof(Node));
  char *s = (char *)malloc(strlen(str) + 1);
  strcpy(s, str);
  p->str = s;
  p->next = begin; 

  return p;  // Now the new element is the first element in the list
}

Node *pop_front(Node *begin)
{
  assert(begin != NULL); // Don't call pop_front() when the list is empty
  Node *p = begin->next;

  free(begin->str);
  free(begin);

  return p;
}

Node *pop_back(Node *begin){
  assert(begin != NULL); // Don't call pop_back() when the list is empty
  if (begin->next == NULL){ // when the list contains only one element
    return pop_front(begin);
  }
  Node *p = begin;
  while (p->next->next != NULL){
    p = p->next;
  }
  free(p->next->str);
  free(p->next);
  p->next = NULL;
  return begin;
}

Node *push_back(Node *begin, const char *str)
{
  if (begin == NULL) {   // If the list is empty
    return push_front(begin, str);
  }

  // Find the last element
  Node *p = begin;
  while (p->next != NULL) {
    p = p->next;
  }

  // Create a new element
  Node *q = (Node *)malloc(sizeof(Node));
  char *s = (char *)malloc(strlen(str) + 1);
  strcpy(s, str);
  q->str = s;
  q->next = NULL;

  // The new element should be linked from the previous last element
  p->next = q;

  return begin;
}

Node *remove_all(Node *begin)
{
  if (begin != NULL){
    while ((begin = pop_front(begin))) {
      ; // Repeat pop_front() until the list becomes empty
    }
  }
  return begin;  // Now, begin is NULL
}

// functions for command
char canvas[WIDTH][HEIGHT];
Node *history = NULL; //配列の代わりにリストで

const char *default_history_file = "history.txt";
const char *default_export_file = "canvas.bmp";

void print_canvas(FILE *fp)
{
  int x, y;

  fprintf(fp, "----------\n");

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      const char c = canvas[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }
  fflush(fp);
}

void init_canvas()
{
  memset(canvas, ' ', sizeof(canvas));
}

int max(const int a, const int b)
{
  return (a > b) ? a : b;
}

void draw_line(const int x0, const int y0, const int x1, const int y1)
{
  int i;
  const int n = max(abs(x1 - x0), abs(y1 - y0));
  for (i = 0; i <= n; i++) {
    const int x = x0 + i * (x1 - x0) / n;
    const int y = y0 + i * (y1 - y0) / n;
    canvas[x][y] = '#';
  }
}

void draw_rectangle(const int x0, const int y0, const int dx, const int dy)
{
  draw_line(x0, y0, x0 + dx, y0);
  draw_line(x0 + dx, y0, x0 + dx, y0 + dy);
  draw_line(x0 + dx, y0 + dy, x0, y0 + dy);
  draw_line(x0, y0 + dy, x0, y0);  
}

void draw_circle(const int x0, const int y0, const int r)
{
  int dx = r, dy = 0;
  while(dx >= dy){
    canvas[x0+dx][y0+dy] = '#';
    canvas[x0+dx][y0-dy] = '#';
    canvas[x0-dx][y0+dy] = '#';
    canvas[x0-dx][y0-dy] = '#';
    canvas[x0+dy][y0+dx] = '#';
    canvas[x0+dy][y0-dx] = '#';   
    canvas[x0-dy][y0+dx] = '#';   
    canvas[x0-dy][y0-dx] = '#';       
    dy++;
    dx = (int)round(sqrt(r*r - dy*dy));
  }
}

void save_history(const char *filename) // history_size は不要
{
  if (filename == NULL){
    filename = default_history_file;
  }
  FILE *fp;
  if ((fp = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return;
  }
  
  // リストからファイルに書き出し
  const Node *p;
  for (p = history; p != NULL; p = p->next) {
    fprintf(fp, "%s", p->str);
  }

  printf("saved as \"%s\"\n", filename);

  fclose(fp);
}

void write_bitmap(const char *filename){
  // bitmap file header
  char bfType[3] = "BM";
  unsigned int bfSize; //
  unsigned short bfReserved1 = 0;
  unsigned short bfReserved2 = 0;
  unsigned int bfOffBits = 14 + 40 + 4 * 2; //
  // bitmap information header
  unsigned int biSize = 40; 
  int biWidth = WIDTH; 
  int biHeight = HEIGHT; 
  unsigned short biPlanes = 1; 
  unsigned short biBitCount = 1; 
  unsigned int biCompression = 0;
  unsigned int biSizeImage = 0; // 0
  unsigned int biXPelsPerMeter = 0; // 0
  unsigned int biYPelsPerMeter = 0; // 0
  unsigned int biClrUsed = 0; // 0 
  unsigned int biClrImportant = 0; 
  // palette data
  char color0[4] = {255,216,178,0}; // B, G, R, 0(reserved) 
  char color1[4] = {127,191,255,0}; //

  int byte_row = ((WIDTH + 31)/ 32) * 4; // 32で割って切り上げ
  bfSize = 14 + 40 + 4*2 + byte_row * HEIGHT;
  
  if (filename == NULL){
    filename = default_export_file;
  }
  FILE *fp;
  if ((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return;
  }
  fwrite(bfType, sizeof(char), 2, fp);
  fwrite(&bfSize, sizeof(unsigned int), 1, fp);
  fwrite(&bfReserved1, sizeof(unsigned short), 1, fp);
  fwrite(&bfReserved2, sizeof(unsigned short), 1, fp);
  fwrite(&bfOffBits, sizeof(unsigned int), 1, fp);

  fwrite(&biSize, sizeof(unsigned int), 1, fp); 
  fwrite(&biWidth, sizeof(int), 1, fp); //
  fwrite(&biHeight, sizeof(int), 1, fp); //
  fwrite(&biPlanes, sizeof(unsigned short), 1, fp); 
  fwrite(&biBitCount, sizeof(unsigned short), 1, fp); 
  fwrite(&biCompression, sizeof(unsigned int), 1, fp);
  fwrite(&biSizeImage, sizeof(unsigned int), 1, fp); // 0
  fwrite(&biXPelsPerMeter, sizeof(unsigned int), 1, fp); // 0
  fwrite(&biYPelsPerMeter, sizeof(unsigned int), 1, fp); // 0
  fwrite(&biClrUsed, sizeof(unsigned int), 1, fp); 
  fwrite(&biClrImportant, sizeof(unsigned int), 1, fp); 

  fwrite(color0, sizeof(char), 4, fp);
  fwrite(color1, sizeof(char), 4, fp);

  char *row = (char *)malloc(byte_row * sizeof(char));
  for (int y = HEIGHT - 1; y >= 0; y--){
    memset(row, 0, byte_row * sizeof(char));
    for (int i = 0; i < byte_row; i++){ // byte
      for (int j = 0; j < 8; j++){ //bit
        if (canvas[i*8 + j][y] == '#'){
          row[i] = row[i] | (0b10000000 >> j);  // 左からj番目のビットを1に
        }      
      }
    }
    fwrite(row, sizeof(char), byte_row, fp);
  }  

  free(row);

  printf("exported to \"%s\"\n", filename);
  fclose(fp);
}

void load_history(const char *filename)
{
  if (filename == NULL){
    filename = default_history_file;
  }
  FILE *fp;
  char buf[BUFSIZE];  
  if ((fp = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return;
  }
  
  history = remove_all(history);
  while(fgets(buf, BUFSIZE, fp)){
    interpret_command(buf);
    history = push_back(history, buf);
  }
  printf("loaded from \"%s\"\n", filename);

  fclose(fp);
}

// Interpret and execute a command
//   return value:
//     0, normal commands such as "line"
//     1, unknown or special commands (not recorded in history[])
//     2, quit
int interpret_command(const char *command)
{
  char buf[BUFSIZE];
  strcpy(buf, command);
  buf[strlen(buf) - 1] = 0; // remove the newline character at the end

  const char *s = strtok(buf, " ");

  if (strcmp(s, "line") == 0) {
    int x0, y0, x1, y1;
    x0 = atoi(strtok(NULL, " "));
    y0 = atoi(strtok(NULL, " "));
    x1 = atoi(strtok(NULL, " "));
    y1 = atoi(strtok(NULL, " "));
    draw_line(x0, y0, x1, y1);
    return 0;
  }

  if (strcmp(s, "rectangle") == 0) { //左上の座標と高さ幅を指定
    int x0, y0, width, height;
    x0 = atoi(strtok(NULL, " "));
    y0 = atoi(strtok(NULL, " "));
    width = atoi(strtok(NULL, " "));
    height = atoi(strtok(NULL, " "));
    draw_rectangle(x0, y0, width, height);
    return 0;
  }

  if (strcmp(s, "circle") == 0) { //中心の座標と半径を指定
    int x0, y0, r;
    x0 = atoi(strtok(NULL, " "));
    y0 = atoi(strtok(NULL, " "));
    r = atoi(strtok(NULL, " "));
    draw_circle(x0, y0, r);
    return 0;
  }

  if (strcmp(s, "save") == 0) {
    s = strtok(NULL, " ");
    save_history(s);
    return 1;
  }

  if (strcmp(s, "load") == 0) {
    s = strtok(NULL, " ");
    load_history(s);
    return 1;
  }

  if (strcmp(s, "export") == 0) {
    s = strtok(NULL, " ");
    write_bitmap(s);
    return 1;
  }

  if (strcmp(s, "undo") == 0) {
    init_canvas();
    /*for (i = 0; i < *hsize - 1; i++) {
      interpret_command(history[i], NULL);
    }
    (*hsize)--;*/
    history = pop_back(history);
    const Node *p;
    for (p = history; p != NULL; p = p->next) {
      interpret_command(p->str);
    }
    return 1;
  }

  if (strcmp(s, "quit") == 0) {
    return 2;
  }

  printf("error: unknown command.\n");

  return 1;
}

int main()
{
  const char *filename = "canvas.txt";
  FILE *fp;
  char buf[BUFSIZE];

  if ((fp = fopen(filename, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return 1;
  }

  init_canvas();
  print_canvas(fp);

  while (1) {
    int hsize = 0;
    const Node *p;
    for (p = history; p != NULL; p = p->next) { // count size of the list
      hsize++;
    }
    printf("%d > ", hsize);
    fgets(buf, BUFSIZE, stdin);

    const int r = interpret_command(buf);
    if (r == 2) break;
    if (r == 0) {
      history = push_back(history, buf);
    }

    print_canvas(fp);
  }

  fclose(fp);

  return 0;
}
