#include "s21_grep.h"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    flags_t Flags = {0};
    str_t Patterns = {0, NULL};
    str_t Filenames = {0, NULL};
    opt_t Opt = {&Flags, &Patterns, &Filenames};

    parser(argc, argv, &Opt);
    my_grep(&Opt);
    clear_all(&Opt);
  }
  return 0;
}

void my_grep(opt_t* Opt) {
  char* mask = regex_mask(Opt->patterns);
  int regcomp_flags = REG_EXTENDED | REG_NEWLINE;
  if (Opt->flags->i) {
    regcomp_flags |= REG_ICASE;
  }
  regex_t preg;
  if (regcomp(&preg, mask, regcomp_flags) == 0) {
    FILE* fp = NULL;
    for (int k = Opt->flags->start_files; k < Opt->filenames->count; k++) {
      fp = fopen(Opt->filenames->text[k], "r");
      if (fp) {
        grep_file(fp, Opt->filenames->text[k], &preg, Opt->flags);
        fclose(fp);
      } else {
        if (Opt->flags->s == 0) {
          fprintf(stderr, "s21_grep: %s: No such file or derectory\n",
                  Opt->filenames->text[k]);
        }
      }
    }
    regfree(&preg);
  }
  if (mask) free(mask);
}

void grep_file(FILE* fp, char* filename, regex_t* preg, flags_t* flags) {
  int line_num = 0;
  int match_lines = 0;
  int do_reading = 1;
  while (do_reading) {
    char new_string[MAX_STR] = {0};
    if (fgets(new_string, MAX_STR, fp)) {
      line_num++;
      size_t nmatch = 1;
      regmatch_t pmatch[1] = {0};
      int match = regexec(preg, new_string, nmatch, pmatch, 0) == flags->v;
      match_lines += match;
      if (match && !flags->c && !flags->l) {
        if (!flags->h) printf("%s:", filename);
        if (flags->n) printf("%d:", line_num);
        if (!flags->o) {
          printf("%s", new_string);
          if (new_string[strlen(new_string) - 1] != '\n') printf("\n");
        } else {
          char* substring = new_string;
          do {
            print_o(pmatch[0].rm_so, pmatch[0].rm_eo, substring);
            substring += pmatch[0].rm_eo;
          } while (!regexec(preg, substring, nmatch, pmatch, 0));
        }
      }
      if (flags->l && match_lines) do_reading = 0;
    } else {
      do_reading = 0;
    }
  }
  if (flags->c) {
    if (!flags->h) printf("%s:", filename);
    printf("%d\n", match_lines);
  }
  if (flags->l && match_lines) {
    printf("%s\n", filename);
  }
}

void print_o(int start, int end, char* string) {
  for (int i = start; i < end; i++) {
    printf("%c", string[i]);
  }
  printf("\n");
}

char* regex_mask(str_t* patterns) {
  char* mask = NULL;
  size_t mask_len = 0;
  for (int i = 0; i < patterns->count; i++) {
    mask = realloc(mask,
                   (mask_len + strlen(patterns->text[i]) + 2) * sizeof(char));
    if (mask) {
      strcpy(mask + mask_len, patterns->text[i]);
      strcat(mask, "|");
      mask_len = strlen(mask);
    }
  }
  if (mask) {
    if (mask[strlen(mask) - 1] == '|') {
      mask[strlen(mask) - 1] = '\0';
    }
  }
  return mask;
}

void parser(int argc, char* argv[], opt_t* Opt) {
  int new_char = 0;
  optind = 1;
  while (optind < argc) {
    new_char = getopt(argc, argv, "e:ivclnhsf:o");
    mapper(new_char, Opt, argv);
  }
  if (Opt->patterns->count == 0 && Opt->filenames->count) {
    add_str(Opt->patterns, Opt->filenames->text[0]);
    Opt->flags->start_files = 1;
  }
  if (Opt->flags->c || Opt->flags->l || Opt->flags->v) {
    Opt->flags->o = 0;
  }
  if ((Opt->filenames->count - Opt->flags->start_files) == 1) {
    Opt->flags->h = 1;
  }
}

void mapper(int new_char, opt_t* Opt, char* argv[]) {
  switch (new_char) {
    case 'i':
      Opt->flags->i = 1;
      break;

    case 'v':
      Opt->flags->v = 1;
      break;

    case 'c':
      Opt->flags->c = 1;
      break;

    case 'l':
      Opt->flags->l = 1;
      break;

    case 'n':
      Opt->flags->n = 1;
      break;

    case 'h':
      Opt->flags->h = 1;
      break;

    case 's':
      Opt->flags->s = 1;
      break;

    case 'o':
      Opt->flags->o = 1;
      break;

    case 'e':
      add_str(Opt->patterns, optarg);
      break;

    case 'f':
      file_patterns(Opt->patterns, optarg);
      break;

    case -1:
      add_str(Opt->filenames, argv[optind]);
      optind++;
      break;
  }
}

void file_patterns(str_t* patterns, char* filename) {
  FILE* fp = NULL;
  fp = fopen(filename, "r");
  if (fp) {
    char new_string[MAX_STR] = {0};
    while (fgets(new_string, MAX_STR, fp)) {
      if (new_string[strlen(new_string) - 1] == '\n') {
        new_string[strlen(new_string) - 1] = '\0';
      }
      if (*new_string) {
        add_str(patterns, new_string);
      }
      memset(new_string, 0, MAX_STR);
    }
    fclose(fp);
  }
}

void add_str(str_t* dest, const char* new_str) {
  dest->text = realloc(dest->text, sizeof(char*) * (dest->count + 1));
  if (dest->text) {
    dest->text[dest->count] = NULL;
    dest->text[dest->count] = calloc(strlen(new_str) + 1, sizeof(char));
    if (dest->text[dest->count]) {
      memset(dest->text[dest->count], 0, strlen(new_str) + 1);
      strcpy(dest->text[dest->count], new_str);
      dest->count += 1;
    }
  }
}

void clear_all(opt_t* Opt) {
  clear_str(Opt->filenames);
  clear_str(Opt->patterns);
}

void clear_str(str_t* dest) {
  for (int i = 0; i < dest->count; i++) {
    if (dest->text[i]) free(dest->text[i]);
  }
  if (dest->text) free(dest->text);
}