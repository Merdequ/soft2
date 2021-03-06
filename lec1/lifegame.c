/*
LIFE GAME
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#define WIDTH 70
#define HEIGHT 40
#define LIVE 'O'
#define DEAD '-'

void print_cells();
void update_cells();
void edit(char cells[HEIGHT][WIDTH]); 
void init_cells();
void save();
void load();
void randinit_cells(int cell_rate);
void autorun(int time);
int cell_count();

int gen = 0;
int oneclock = 50; //[ms]
char cells[HEIGHT][WIDTH]; //左右上下端は常にDEAD

int main(void){
  char input[20]={},cmd[10]={};
  int autotime = 100, cellrate = 10, argnum, arg1, arg2;

  randinit_cells(20);
  print_cells();

  
  // main loop
  while(1){
    printf("\n (run[repeat time, clock(ms)] / save / load / edit / random[cell rate(%%)] / q )\n>> ");
    input[0] = '\0';
    if (strcmp(cmd, "run") != 0){ //実行は連打を許可
      cmd[0] = '\0';
    } 
    // input process
    fgets(input, sizeof(input), stdin);
    if ( input[strlen(input)-1] != '\n' ){
      while( getchar() != '\n' ); // clear buffer
    }
    argnum = sscanf(input, "%9s %d %d", cmd, &arg1, &arg2);
    
    if(strcmp(cmd, "q") == 0){
      break;
    }
    //実行
    if(strcmp(cmd, "run") == 0){
      if (argnum >= 2){
        if (arg1 > 0){
          autotime = arg1;
        }
        else{
          printf("warning: repetition time > 0\n");
          continue;
        }
        if (argnum >= 3){
          if (arg2 >= 10 && arg2 < 1000){
            oneclock = arg2;
          }
          else{
            printf("warning: 10 <= clock[ms] <= 999\n");
            continue;
          }
        } 
      }
      autorun(autotime);
      continue;
    }
    //編集モード
    if(strcmp(cmd, "edit") == 0){
      edit(cells);
      print_cells();
      continue;
    }
    if (strcmp(cmd, "random") == 0){ //リセット
    	 if (argnum >= 2){
    	   if (arg1 >= 0 && arg1 <= 100){
    	     cellrate = arg1;
    	   }
    	   else{
    	     printf("warning: 0 <= cell rate <= 100");
    	   }
    	 } 
      randinit_cells(cellrate);
      print_cells();
      printf("initialized!\n");
      continue;
    }
    //ファイルに保存
    if(strcmp(cmd, "save") == 0){
      save();
      continue;
    }
    //読み込み
    if(strcmp(cmd, "load") == 0){
      load();
      continue;
    }

    // unexpected
    printf("\ninvalid %d\n",argnum);
  }
  printf("End\n");
  return 0;
}


void print_cells(){
  int i, j;
  system("clear");
  for(i = 1; i < HEIGHT - 1; i++){
    for(j = 1; j < WIDTH - 1; j++){
      printf("%c ", cells[i][j]);
    }
    printf("\n");
  }  
}

void update_cells(){
  int i, j, cnt;
  char temp[HEIGHT][WIDTH]; //逐次書き換えるとバグる。一時保存しておいてまとめて書き換える。

  //judge
  for(i = 1; i < HEIGHT - 1; i++){
    for(j = 1; j < WIDTH - 1; j++){
      cnt = 0;

      if(cells[i-1][j-1] == LIVE) {cnt++;}
      if(cells[i-1][j  ] == LIVE) {cnt++;}
      if(cells[i-1][j+1] == LIVE) {cnt++;}
      if(cells[i  ][j-1] == LIVE) {cnt++;}
      if(cells[i  ][j+1] == LIVE) {cnt++;}
      if(cells[i+1][j-1] == LIVE) {cnt++;}
      if(cells[i+1][j  ] == LIVE) {cnt++;}
      if(cells[i+1][j+1] == LIVE) {cnt++;}
      
      // 判定ルール
      if(cells[i][j] == DEAD){
        if(cnt == 3){
          temp[i][j] = LIVE;
        }
        else{
          temp[i][j] = DEAD;
        }
      }
      else{
        if(cnt <= 1 || cnt >= 4){
          temp[i][j] = DEAD;
        }
        else{
          temp[i][j] = LIVE;
        }
      }      
    }
  }
  //書き換え
  for(i = 1; i < HEIGHT - 1; i++){
    for(j = 1; j < WIDTH - 1; j++){
      cells[i][j] = temp[i][j];
    }
  }  
}

void edit(char cells[HEIGHT][WIDTH]){
  int x = 0, y = 0; 
  char c, temp;
  while(1){
    temp = cells[y + 1][x + 1];
    cells[y + 1][x + 1] = '*';
    printf("\n");
    print_cells();
    cells[y + 1][x + 1] = temp;
    printf("\nEdit mode\n\nwasd: move cursor(*) / e: select / q: exit\n>> ");
    c = getchar();
    switch(c){
    case 'q':
    	 getchar(); // buffer clear
      return;
      break;
    case 'd': //right
      x = (x + 1) % (WIDTH - 2);
      break;
    case 'a': //left
      x = (x + (WIDTH - 2) - 1) % (WIDTH - 2);
      break;
    case 's': //down
      y = (y + 1) % (HEIGHT - 2);
      break;      
    case 'w': //up
      y = (y + (HEIGHT - 2) - 1) % (HEIGHT - 2);
      break;
    case 'e': //select
      cells[y + 1][x + 1] = (cells[y + 1][x + 1] == LIVE)? DEAD : LIVE;
      break;
    }
  }
}

void randinit_cells(int cell_rate){ // active cell rate[%]
	gen = 0;
  srand((unsigned)time(NULL));
  for(int i = 1; i < HEIGHT - 1; i++){
    for(int j = 1; j < WIDTH - 1; j++){
      cells[i][j] = (rand()%100 < cell_rate) ? LIVE : DEAD;
    }
  }  
}

void load(){
  FILE *fp; 
  printf("\nFile to load >> ");
  char filename[32];
  scanf("%s", filename);
  while( getchar() != '\n' );
  // clear file
  if ((fp = fopen(filename, "r")) == NULL){
    printf("Failed to open the file '%s'\n", filename);
    return;
  }

  int i,j;
  for(i = 0; i < HEIGHT; i++){
    for(j = 0; j < WIDTH; j++){
      cells[i][j] = DEAD;
    }
  }
  //read from file
  fscanf(fp, "gen %d", &gen);
  while(fscanf(fp, "%d %d", &i, &j) != EOF){
    cells[i][j] = LIVE;
  }
  fclose(fp);
  print_cells();
  printf("Loaded from the file '%s'\n", filename);
  return;  
}

void save(){
  FILE *fp; 
  printf("\nFile to write >> ");
  char filename[32];
  scanf("%s", filename);
  while( getchar() != '\n' );
  // clear file
  if ((fp = fopen(filename, "w")) == NULL){
    printf("Failed to write in the file '%s'\n", filename);
    return;
  }
  fclose(fp);
  //write to file
  fp = fopen(filename, "a");
  fprintf(fp,"gen %d\n", gen);
  for(int i = 1; i < HEIGHT-1; i++){
    for(int j = 1; j < WIDTH-1; j++){
      if(cells[i][j] == LIVE){
        fprintf(fp,"%d %d\n", i,j);
      }
    }
  }
  fclose(fp);
  printf("Written to the file '%s'\n", filename);
  return;
}

void autorun(int time){
  struct timespec req = {0, oneclock * 1000000};
  
  // 実行
  for (int t = 0; t < time; t++){
		gen++;
		update_cells();
    int area = (WIDTH - 1) * (HEIGHT - 1);
    int active = cell_count();
    double percentage = (double)active * 100 / area; 
	  print_cells();
	  printf("\nclock : %d[ms]\n", oneclock);
	  printf("gen : %d\n", gen);
    printf("active cell : %d/%d = %f%%\n", active, area, percentage);
	  nanosleep(&req, NULL);
  }
}

int cell_count(){
  int cnt = 0;
  for (int i = 1; i<HEIGHT - 1; i++){
    for (int j = 1; j<WIDTH - 1; j++){
      if (cells[i][j] == LIVE){
        cnt++;
      }
    }
  }
  return cnt;
}
