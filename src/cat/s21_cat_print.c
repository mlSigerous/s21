#include "s21_cat.h"

void process_line_start(int c, const int* start, int* row, const Flags* flags) {
  if (*start) {
    if (flags->b && c != '\n') {
      printf("%6d\t", (*row)++);
    } else if (flags->n && !flags->b) {
      printf("%6d\t", (*row)++);
    }
  }
}

int process_char(int* c, int next, const Flags* flags) {
  int is_char_printed = 0;
  if (flags->t && *c == '\t') {
    printf("^I");
    is_char_printed = 1;
  }
  if (flags->v) {
    if (*c >= 128) {
      printf("M-");
      *c -= 128;
    }
    if (*c == 127) {
      printf("^?");
      is_char_printed = 1;
    } else if (*c >= 0 && *c <= 31 && *c != '\n' && *c != '\t') {
      printf("^%c", *c + 64);
      is_char_printed = 1;
    }
  }
  if (flags->e) {
    if (*c == '\r' && next == '\n' && !flags->v) {
      printf("^M");
      is_char_printed = 1;
    } else if (*c == '\n') {
      printf("$");
    }
  }
  return is_char_printed;
}

void update_state(int c, int* start) {
  if (c == '\n') {
    *start = 1;
  } else {
    *start = 0;
  }
}

int squeeze_blank(int c, const int* start, int* empty_rows,
                  const Flags* flags) {
  int flag = 0;
  if (flags->s && *start && c == '\n') {
    (*empty_rows)++;
    if (*empty_rows > 1) {
      flag = 1;
    }
  } else if (c != '\n') {
    *empty_rows = 0;
  }
  return flag;
}

void print_file(FILE* file, const Flags* flags, int* row, int* start,
                int* empty_rows) {
  int c;
  while ((c = fgetc(file)) != EOF) {
    int next = fgetc(file);
    int squeeze = squeeze_blank(c, start, empty_rows, flags);
    if (!squeeze) {
      process_line_start(c, start, row, flags);
      int flag = process_char(&c, next, flags);
      if (!flag) {
        putchar(c);
      }
    }
    update_state(c, start);
    if (next != EOF) {
      ungetc(next, file);
    }
  }
}