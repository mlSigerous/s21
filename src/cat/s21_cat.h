#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <string.h>

typedef struct {
  int b;
  int v;
  int e;
  int n;
  int s;
  int t;
} Flags;

int is_flag(const char* arg);
int is_file(const char* arg);
int is_long(const char* arg);
int parse_long_flags(const char* arg, Flags* flags);
int parse_short_flags(const char* arg, Flags* flags);
int parse_flags(const char* arg, Flags* flags);
int process_options(int argc, char* argv[], Flags* flags);
int process_files(int argc, char* argv[], const Flags* flags, int* row,
                  int* start, int* empty_rows);
void process_line_start(int c, const int* start, int* row, const Flags* flags);
int process_char(int* c, int next, const Flags* flags);
void update_state(int c, int* start);
int squeeze_blank(int c, const int* start, int* empty_rows, const Flags* flags);
void print_file(FILE* file, const Flags* flags, int* row, int* start,
                int* empty_rows);

#endif