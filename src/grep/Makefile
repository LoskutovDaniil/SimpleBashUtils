CC = gcc
FLAGS = -Wall -Werror -Wextra

all: s21_grep
	

s21_grep:
	$(CC) $(FLAGS) s21_grep.c -o s21_grep
	
test:
	sh tests.sh

leak:
	leaks -atExit -- ./s21_grep

style:
	clang-format -n *.c *.h

clean: 
	rm -rf s21_grep