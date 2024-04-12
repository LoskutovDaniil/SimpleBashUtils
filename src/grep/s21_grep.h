#ifndef GREP_H
#define GREP_H

#define MAX_STR 2000

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int i;  // Игнорирует различия регистра.
  int v;  // Выводит строки из файла 1.txt, где нет аргумента
  int c;  // Выводит только количество совпадающих строк.
  int l;  // Выводит только совпадающие файлы.
  int n;  // Предваряет каждую строку вывода номером строки из файла ввода.
  int h;  // Выводит содержимое совпадающих строк без имен файлов
  int s;  // Подавляет сообщения об ошибках о несуществующих файлах.
  int o;  // Печатает только совпадающие (непустые) части совпавшей строки.
  int start_files;  // Начало файлов
} flags_t;

typedef struct {
  int count;    // и их кол - во
  char** text;  //строки
} str_t;

typedef struct {
  flags_t* flags;
  str_t* patterns;
  str_t* filenames;
} opt_t;  //общая структура всех структур

void parser(int argc, char* argv[], opt_t* Opt);
void mapper(int new_char, opt_t* Opt, char* argv[]);
void file_patterns(str_t* patterns, char* filename);

void add_str(str_t* dest, const char* new_str);
void clear_all(opt_t* Opt);
void clear_str(str_t* dest);

void my_grep(opt_t* Opt);

void grep_file(FILE* fp, char* filename, regex_t* preg, flags_t* flags);

void print_o(int start, int end, char* string);

char* regex_mask(str_t* patterns);

void DePrinter(opt_t* Opt, char* mask);

#endif
