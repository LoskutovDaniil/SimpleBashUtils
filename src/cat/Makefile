CC = gcc
FLAGS = -Wall -Werror -Wextra

all: s21_cat
	

s21_cat:
	$(CC) $(FLAGS) s21_cat.c -o s21_cat
	
test:
	sh tests.sh

style:
	clang-format -n *.c *.h

clean: 
	rm -rf s21_cat