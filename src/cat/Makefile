CC=gcc
CFLAGS=-Wall -Wextra -Werror
SRC=s21_cat.c s21_cat_parse.c s21_cat_print.c

s21_cat: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o s21_cat

clean:
	rm -f s21_cat

valgrind: s21_cat
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=1 \
	         ./s21_cat test.txt

.PHONY: clean