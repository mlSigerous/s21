CC=gcc
CFLAGS=-Wall -Wextra -Werror
SRC=s21_grep.c s21_grep_parse.c s21_grep_regex.c s21_grep_print.c

s21_grep: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o s21_grep

clean:
	rm -f s21_grep

valgrind: s21_grep
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=1 \
	         ./s21_grep "hello" test.txt

.PHONY: clean