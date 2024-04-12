#include "s21_cat.h"

int main(int argc, char* argv[]) {
  if (argc > 1) {
    flags_t Flags = {0};
    parser(argc, argv, &Flags);
    my_cat(argc, argv, &Flags);
  }
  return 0;
}

void parser(int argc, char* argv[], flags_t* Flags) {
  struct option long_option[] = {{"number-nonblank", 0, NULL, 'b'},
                                 {"number", 0, NULL, 'n'},
                                 {"squeeze-blank", 0, NULL, 's'},
                                 {NULL, 0, NULL, 0}};
  for (int new_opt = getopt_long(argc, argv, "+beEnstTv", long_option, 0);
       new_opt != -1;
       new_opt = getopt_long(argc, argv, "+beEnstTv", long_option, 0)) {
    switch (new_opt) {
      case 'b':
        Flags->b = 1;
        break;
      case 'v':
        Flags->v = 1;
        break;
      case 'e':
        Flags->e = Flags->v = 1;
        break;
      case 'E':
        Flags->e = 1;
        break;
      case 'n':
        Flags->n = 1;
        break;
      case 's':
        Flags->s = 1;
        break;
      case 't':
        Flags->t = Flags->v = 1;
        break;
      case 'T':
        Flags->t = 1;
        break;
    }
  }
}

void my_cat(int argc, char* argv[], flags_t* Flags) {
  FILE* fp = NULL;
  for (int i = optind; i < argc; i++) {
    fp = fopen(argv[i], "r");
    if (fp) {
      cat_file(fp, Flags);
      fclose(fp);
    } else {
      fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], argv[i]);
    }
  }
}

void cat_file(FILE* fp, flags_t* Flags) {
  int count_str = 0;
  int last_symbol = '\n';
  int new_symbol = fgetc(fp);

  while (new_symbol != EOF) {
    if (Flags->s && last_symbol == '\n' && new_symbol == '\n') {
      while (new_symbol == '\n') new_symbol = fgetc(fp);
      if (new_symbol != EOF) fseek(fp, -1, SEEK_CUR);
      new_symbol = '\n';
    }
    if (last_symbol == '\n')
      if ((Flags->n && !Flags->b) || (Flags->b && new_symbol != '\n'))
        printf("%6d\t", ++count_str);

    print_symbol(new_symbol, Flags);
    last_symbol = new_symbol;
    new_symbol = fgetc(fp);
  }
}

void print_symbol(int symbol, flags_t* Flags) {
  int ready_to_print = 1;
  if (symbol == EOF) {
    ready_to_print = 0;
  }
  if (symbol == '\n' && Flags->e) {
    printf("$\n");
    ready_to_print = 0;
  }
  if (symbol == '\t' && Flags->t) {
    printf("^I");
    ready_to_print = 0;
  }
  if (Flags->v && ready_to_print) {
    if (symbol == 127) {
      printf("^?");
      ready_to_print = 0;
    }
    if (symbol < 32 && symbol != '\n' && symbol != '\t') {
      printf("^%c", symbol + 64);
      ready_to_print = 0;
    }
    if (symbol > 127 && symbol < 160) {
      printf("M-^%c", symbol - 64);
      ready_to_print = 0;
    }
  }
  if (ready_to_print) printf("%c", symbol);
}