#ifndef CAT_H
#define CAT_H

#include <getopt.h>
#include <stdio.h>

typedef struct {
  int b;
  int n;
  int e;
  int s;
  int t;
  int v;
} flags_t;

void parser(int argc, char* argv[], flags_t* Flags);
void my_cat(int argc, char* argv[], flags_t* Flags);
void cat_file(FILE* fp, flags_t* Flags);
void print_symbol(int symbol, flags_t* Flags);

#endif
