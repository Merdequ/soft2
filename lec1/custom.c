/*
LIFE GAME
with 2 type creature
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
#define ENEMY 'X'
#define EMPTY '-'

void print_cells();
void update_cells();
void edit(char cells[HEIGHT][WIDTH]); 
void init_cells();
void save();
void load();
void randinit_cells(int cellrate, int typerate, int mixinglevel);
void autorun(int time);
void cell_count(int cnt[2]);

int gen = 0;
int oneclock = 50; //[ms]
int emergerate = 60; //競合時にtype1が誕生する
char cells[HEIGHT][WIDTH]; //左右上下端は常にEMPTY

int main(void){
  char input[20]={},cmd[10]={};
  int autotime = 100, cellrate = 10, typerate = 50, mixinglevel = 25, argnum, arg1, arg2, arg3;

  srand((unsigned)time(NULL));
  randinit_cells(cellrate, typerate,mixinglevel);
  print_cells();

  
  // main loop
  while(1){
    printf("\n (run[repeat time, clock(ms), emergerate(%%)] / save / load / edit / random[cell rate(%%), type rate(%%), mixing level(%%)] / q )\n>> ");
    input[0] = '\0';
    if (strcmp(cmd, "run") != 0){ //実行は連打を許可
      cmd[0] = '\0';
    } 
    // input process
    fgets(input, sizeof(input), stdin);
    if ( input[strlen(input)-1] != '\n' ){
      while( getchar() != '\n' ); // clear buffer
    }
    argnum = sscanf(input, "%9s %d %d %d", cmd, &arg1, &arg2, &arg3);
    
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
      if (argnum >= 4){
	if (arg3 >= 0 && arg3 <=100 ){
	  emergerate = arg3;
	}
	else{
	  printf("warning: 0 <= emergence rate <= 100\n");
	  continue;
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
    // リセット
    if (strcmp(cmd, "random") == 0){ 
      if (argnum >= 2){
	if (arg1 >= 0 && arg1 <= 100){
	  cellrate = arg1;
	}
	else{
	  printf("warning: 0 <= cell rate <= 100");
	  continue;
	}
      } 
      if (argnum >= 3){
	if (arg2 >=0 && arg2 <= 100){
	  typerate = arg2;
	}
	else {
	  printf("warning: 0 <= type1 percentage <= 100");
	  continue;
	}
      }
      if (argnum >= 4){
	if (arg3 >=0 && arg3 <= 100){
	  mixinglevel = arg3;
	}
	else {
	  printf("warning: 0 <= type1 percentage <= 100");
	  continue;
	}
      }
      randinit_cells(cellrate, typerate, mixinglevel);
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
  int i, j, cnt, cnt2, sum;
  char temp[HEIGHT][WIDTH]; //逐次書き換えるとバグる。一時保存しておいてまとめて書き換える。

  //judge
  for(i = 1; i < HEIGHT - 1; i++){
    for(j = 1; j < WIDTH - 1; j++){
      cnt = 0;
      cnt2 = 0;

      if(cells[i-1][j-1] == LIVE) {cnt++;}
      if(cells[i-1][j  ] == LIVE) {cnt++;}
      if(cells[i-1][j+1] == LIVE) {cnt++;}
      if(cells[i  ][j-1] == LIVE) {cnt++;}
      if(cells[i  ][j+1] == LIVE) {cnt++;}
      if(cells[i+1][j-1] == LIVE) {cnt++;}
      if(cells[i+1][j  ] == LIVE) {cnt++;}
      if(cells[i+1][j+1] == LIVE) {cnt++;}

      if(cells[i-1][j-1] == ENEMY) {cnt2++;}
      if(cells[i-1][j  ] == ENEMY) {cnt2++;}
      if(cells[i-1][j+1] == ENEMY) {cnt2++;}
      if(cells[i  ][j-1] == ENEMY) {cnt2++;}
      if(cells[i  ][j+1] == ENEMY) {cnt2++;}
      if(cells[i+1][j-1] == ENEMY) {cnt2++;}
      if(cells[i+1][j  ] == ENEMY) {cnt2++;}
      if(cells[i+1][j+1] == ENEMY) {cnt2++;}
      
      sum = cnt + cnt2;
      // 判定ルール

      if (cnt2 == 0){ // only type1
	if(cells[i][j] == EMPTY){
	  if(cnt == 3){
	    temp[i][j] = LIVE;
	  }
	  else{
	    temp[i][j] = EMPTY;
	  }
	}
	else{
	  if(cnt <= 1 || cnt >= 4){
	    temp[i][j] = EMPTY;
	  }
	  else{
	    temp[i][j] = LIVE;
	  }
	}
      }
      else if (cnt == 0){ // only type2
	if(cells[i][j] == EMPTY){
	  if(cnt2 == 3){
	    temp[i][j] = ENEMY;
	  }
	  else{
	    temp[i][j] = EMPTY;
	  }
	}
	else{
	  if(cnt2 <= 1 || cnt2 >= 4){
	    temp[i][j] = EMPTY;
	  }
	  else{
	    temp[i][j] = ENEMY;
	  }
	}
      }
      else{ //mix
	if(cells[i][j] == EMPTY){
	  if(sum == 2 || sum == 3){
	    temp[i][j] = (rand() % 100 < emergerate)? LIVE:ENEMY;
	  }
	  else{
	    temp[i][j] = EMPTY;
	  }
	}
	else if (cells[i][j] == LIVE){ //type1
	  if(sum <= 1 || sum >= 4){
	    temp[i][j] = EMPTY;
	  }
	  else{
	    temp[i][j] = LIVE;
	  }
	}
	else{ // TYPE2
	  if(sum <= 1 || sum >= 4){
	    temp[i][j] = EMPTY;
	  }
	  else{
	    temp[i][j] = ENEMY;
	  }
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
    printf("\nEdit mode\n\nwasd: move cursor(*) / c: type1 / x: type2 / e: empty / q: exit\n>> ");
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
    case 'e': //empty
      cells[y + 1][x + 1] = EMPTY;
      break;
    case 'c' :
      cells[y + 1][x + 1] = LIVE;
      break;
    case 'x' :
      cells[y + 1][x + 1] = ENEMY;
    }
  }
}

void randinit_cells(int cellrate, int typerate, int mixinglevel){ // active cell rate[%]
  gen = 0;
  double x; // 位置 0~1
  double px, px1; 
  for(int i = 1; i < HEIGHT - 1; i++){
    for(int j = 1; j < WIDTH - 1; j++){
      x = (double)j / WIDTH;
      px1 = (x < (double)typerate/100)? 1 : 0;
      px = (mixinglevel * (double)typerate/100 + (100-mixinglevel) * px1) / 100;
      cells[i][j] = (rand()%100 >= cellrate) ? EMPTY : ((double)(rand())/RAND_MAX<px)? LIVE : ENEMY;
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
  char c;
  for(i = 0; i < HEIGHT; i++){
    for(j = 0; j < WIDTH; j++){
      cells[i][j] = EMPTY;
    }
  }
  //read from file
  fscanf(fp, "gen %d", &gen);
  while(fscanf(fp, "%d %d %c", &i, &j, &c) != EOF){
    cells[i][j] = c;
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
      if(cells[i][j] != EMPTY){
        fprintf(fp,"%d %d %c\n", i,j,cells[i][j]);
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
    int count[2];
    cell_count(count);
    double percentage1 = (double)(count[0]) * 100 / area; 
    double percentage2 = (double)(count[1]) * 100 / area; 
    print_cells();
    printf("\nclock : %d[ms]\n", oneclock);
    printf("gen : %d\n", gen);
    printf("Emergence rate - Type1: %d%%, Type2: %d%%\n", emergerate,100-emergerate);
    printf("Type1 : %d/%d = %f%%\n", count[0], area, percentage1);
    printf("Type2 : %d/%d = %f%%\n", count[1], area, percentage2);
	  nanosleep(&req, NULL);
  }
}

void cell_count(int cnt[2]){
  cnt[0] = 0;
  cnt[1] = 0;
  for (int i = 1; i<HEIGHT - 1; i++){
    for (int j = 1; j<WIDTH - 1; j++){
      if (cells[i][j] == LIVE){
        (cnt[0])++;
      }
      else if (cells[i][j] == ENEMY){
	(cnt[1])++;
      }
    }
  }
  return;
}
