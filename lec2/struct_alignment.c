/*
課題1
構造体中のメンバ変数に割り当てられているアドレスを表示

どのようなアラインメントが起きているかを考察：
age(int) の後に height(double) が来るところ(3箇所)と，
tag(char)の後にid(int)が来るところ以外はすき間なく詰まっているようである。
パディングがされているところを見ると，
int型のアドレスは 4 の倍数に， double型のアドレスは 8 の倍数になるように
配置されていると思われる。
*/

#include <stdio.h>

struct student
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

struct tagged_student1
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
};

struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

int main (int argc, char**argv)
{
  struct student s;
  struct tagged_student1 s1;
  struct tagged_student2 s2;

  printf("%-12sのアドレスは, %p です\n", "s.id", &s.id);
  printf("%-12sのアドレスは, %p です\n", "s.name", &s.name);
  printf("%-12sのアドレスは, %p です\n", "s.age", &s.age);  
  printf("%-12sのアドレスは, %p です\n", "s.height", &s.height);
  printf("%-12sのアドレスは, %p です\n", "s.weight", &s.weight);
  
  printf("%-12sのアドレスは, %p です\n", "s1.id", &s1.id);
  printf("%-12sのアドレスは, %p です\n", "s1.name", &s1.name);
  printf("%-12sのアドレスは, %p です\n", "s1.age", &s1.age);
  printf("%-12sのアドレスは, %p です\n", "s1.height", &s1.height);
  printf("%-12sのアドレスは, %p です\n", "s1.weight", &s1.weight);
  printf("%-12sのアドレスは, %p です\n", "s1.tag", &s1.tag);
  
  printf("%-12sのアドレスは, %p です\n", "s2.tag", &s2.tag);  
  printf("%-12sのアドレスは, %p です\n", "s2.id", &s2.id);
  printf("%-12sのアドレスは, %p です\n", "s2.name", &s2.name);
  printf("%-12sのアドレスは, %p です\n", "s2.age", &s2.age);
  printf("%-12sのアドレスは, %p です\n", "s2.height", &s2.height);
  printf("%-12sのアドレスは, %p です\n", "s2.weight", &s2.weight);

  printf("\n");
  printf("%-17d %-17d %-17d %-17d %-17d\n", sizeof(s.id), sizeof(s.name), sizeof(s.age), sizeof(s.height), sizeof(s.weight));  
  printf("%-17u %-17u %-17u %-17u %-17u\n", &s.id, &s.name, &s.age, &s.height, &s.weight);

  printf("%-17d %-17d %-17d %-17d %-17d %-17d\n", sizeof(s1.id), sizeof(s1.name), sizeof(s1.age), sizeof(s1.height), sizeof(s1.weight), sizeof(s1.tag));  
  printf("%-17u %-17u %-17u %-17u %-17u %-17u\n", &s1.id, &s1.name, &s1.age, &s1.height, &s1.weight, &s1.tag);

  printf("%-17d %-17d %-17d %-17d %-17d %-17d\n", sizeof(s2.tag), sizeof(s2.id), sizeof(s2.name), sizeof(s2.age), sizeof(s2.height), sizeof(s2.weight));  
  printf("%-17u %-17u %-17u %-17u %-17u %-17u\n", &s2.tag, &s2.id, &s2.name, &s2.age, &s2.height, &s2.weight);
  
  return 0;
}
