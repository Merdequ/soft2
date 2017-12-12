#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define WIDTH 70
#define HEIGHT 40

// #define MAX_HISTORY 100
#define BUFSIZE 1000


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
  while ((begin = pop_front(begin))) 
    ; // Repeat pop_front() until the list becomes empty
  return begin;  // Now, begin is NULL
}

// functions for command
char canvas[WIDTH][HEIGHT];
// char history[MAX_HISTORY][BUFSIZE];
Node *history = NULL; //配列の代わりにリストで

const char *default_history_file = "history.txt";

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

void save_history(const char *filename) // history_size は不要
{
  if (filename == NULL)
    filename = default_history_file;
  
  FILE *fp;
  if ((fp = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", filename);
    return;
  }
  
  // リストからファイルに書き出し
  /*
  int i;
  for (i = 0; i < history_size; i++) {
    fprintf(fp, "%s", history[i]);
  }*/
  const Node *p;
  for (p = history; p != NULL; p = p->next) {
    fprintf(fp, "%s", p->str);
  }

  printf("saved as \"%s\"\n", filename);

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

  if (strcmp(s, "save") == 0) {
    s = strtok(NULL, " ");
    save_history(s);
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
      // strcpy(history[hsize], buf);
    }

    print_canvas(fp);
  }

  fclose(fp);

  return 0;
}
