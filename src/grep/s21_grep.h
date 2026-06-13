#ifndef S21_GREP_H
#define S21_GREP_H

#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} Flags;

typedef struct {
  char** patterns;
  int cnt;
  int allocated;
} PatternsList;

void init_patterns(PatternsList* patterns);
int add_pattern(const char* pattern, PatternsList* patterns);
int get_patterns(const char* filename, PatternsList* patterns);
void free_patterns(PatternsList* patterns);
void free_patterns_o(regex_t* re_o, int re_o_cnt);
int process_getopt_error(int opt);
int process_flag(int opt, Flags* flags, PatternsList* patterns);
int parse_flags(int argc, char** argv, Flags* flags, PatternsList* patterns);
int validate_patterns(PatternsList* patterns, const Flags* flags);
int match_patterns(char* line, PatternsList* patterns, const Flags* flags,
                   int* error);
int compile_patterns_o(PatternsList* patterns, const Flags* flags,
                       regex_t** re_o, int* re_o_cnt);
void print_flag_o(const Flags* flags, regex_t* re, char* line,
                  const char* filename, int row, int files_cnt);
void print_usage(void);
void print_match(const Flags* flags, regex_t* re_o, int re_o_cnt, char* line,
                 const char* filename, int row, int files_cnt, int* vo_active);
void print_cnt(const Flags* flags, const char* filename, int files_cnt,
               int match_cnt);
int process_line(char* line, Flags* flags, PatternsList* patterns, int* error);
void trim_newline(char* line);
int print_regexp(FILE* fp, Flags* flags, const char* filename, int files_cnt,
                 PatternsList* patterns, regex_t* re_o, int re_o_cnt,
                 int* error);
void process_files(int argc, char* argv[], int files_cnt, Flags* flags,
                   PatternsList* patterns, regex_t* re_o, int re_o_cnt,
                   int* found, int* error, int* file_error);

#endif