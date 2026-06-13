#include "s21_grep.h"

int match_patterns(char* line, PatternsList* patterns, const Flags* flags,
                   int* error) {
  int found = 0;
  int ignorecase = 0;
  if (flags->i) {
    ignorecase = REG_ICASE;
  }
  for (int i = 0; i < patterns->cnt && !*error && !found; i++) {
    regex_t re = {0};
    int rc = 0;
    rc = regcomp(&re, patterns->patterns[i], ignorecase);
    if (rc != 0) {
      char error_buff[256];
      regerror(rc, &re, error_buff, sizeof(error_buff));
      fprintf(stderr, "grep: %s\n", error_buff);
      *error = 2;
    } else {
      if (regexec(&re, line, 0, NULL, 0) == 0) {
        found = 1;
      }
      regfree(&re);
    }
  }
  return found;
}

int compile_patterns_o(PatternsList* patterns, const Flags* flags,
                       regex_t** re_o, int* re_o_cnt) {
  int error = 0;
  int reg_flags = 0;
  if (flags->i) {
    reg_flags = REG_ICASE;
  }
  *re_o_cnt = patterns->cnt;
  *re_o = calloc((size_t)patterns->cnt, sizeof(regex_t));
  if (*re_o == NULL) {
    error = 2;
  }
  for (int i = 0; i < patterns->cnt && !error; i++) {
    int rc = regcomp(&(*re_o)[i], patterns->patterns[i], reg_flags);
    if (rc != 0) {
      char error_buff[256];
      regerror(rc, &(*re_o)[i], error_buff, sizeof(error_buff));
      fprintf(stderr, "grep: %s\n", error_buff);
      error = 2;
    }
  }
  return error;
}