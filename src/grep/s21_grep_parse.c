#include "s21_grep.h"

void init_patterns(PatternsList* patterns) {
  patterns->patterns = malloc(32 * sizeof(char*));
  patterns->cnt = 0;
  patterns->allocated = 32;
}

int add_pattern(const char* pattern, PatternsList* patterns) {
  int error = 0;
  if (patterns->cnt >= patterns->allocated) {
    int alloc = patterns->allocated * 2;
    char** tmp = realloc(patterns->patterns, sizeof(char*) * alloc);
    if (tmp != NULL) {
      patterns->patterns = tmp;
      patterns->allocated = alloc;
    } else {
      error = 1;
    }
  }
  if (!error) {
    char* copy = strdup(pattern);
    if (copy != NULL) {
      patterns->patterns[patterns->cnt++] = copy;
    } else {
      error = 1;
    }
  }
  return error;
}

int get_patterns(const char* filename, PatternsList* patterns) {
  int error = 0;
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    error = 2;
  }
  char* line = NULL;
  size_t capacity = 0;
  while (!error && getline(&line, &capacity, fp) != -1) {
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }
    error = add_pattern(line, patterns);
  }
  if (fp != NULL) {
    fclose(fp);
  }
  free(line);
  return error;
}

void free_patterns(PatternsList* patterns) {
  for (int i = 0; i < patterns->cnt; i++) {
    free(patterns->patterns[i]);
  }
  free(patterns->patterns);
}

void free_patterns_o(regex_t* re_o, int re_o_cnt) {
  if (re_o != NULL) {
    for (int i = 0; i < re_o_cnt; i++) {
      regfree(&re_o[i]);
    }
    free(re_o);
  }
}

int process_getopt_error(int opt) {
  int error = 0;
  if (opt == '?') {
    fprintf(stderr, "grep: invalid option -- '%c'\n", optopt);
    error = 2;
  } else if (opt == ':') {
    fprintf(stderr, "grep: option requires an argument -- '%c'\n", optopt);
    print_usage();
    error = 2;
  }
  return error;
}

int process_flag(int opt, Flags* flags, PatternsList* patterns) {
  int error = 0;
  switch (opt) {
    case 'e':
      flags->e = 1;
      error = add_pattern(optarg, patterns);
      break;
    case 'i':
      flags->i = 1;
      break;
    case 'v':
      flags->v = 1;
      break;
    case 'c':
      flags->c = 1;
      break;
    case 'l':
      flags->l = 1;
      break;
    case 'n':
      flags->n = 1;
      break;
    case 'h':
      flags->h = 1;
      break;
    case 's':
      flags->s = 1;
      break;
    case 'f':
      flags->f = 1;
      error = get_patterns(optarg, patterns);
      break;
    case 'o':
      flags->o = 1;
      break;
    default:
      error = process_getopt_error(opt);
      break;
  }
  return error;
}

int parse_flags(int argc, char** argv, Flags* flags, PatternsList* patterns) {
  int error = 0;
  int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, ":e:ivclnhsf:o")) != -1 && !error) {
    error = process_flag(opt, flags, patterns);
  }
  return error;
}

int validate_patterns(PatternsList* patterns, const Flags* flags) {
  int error = 0;
  int reg_flags = 0;
  if (flags->i) {
    reg_flags = REG_ICASE;
  }
  for (int i = 0; i < patterns->cnt && !error; i++) {
    regex_t re = {0};
    int rc = regcomp(&re, patterns->patterns[i], reg_flags);
    if (rc != 0) {
      char error_buff[256];
      regerror(rc, &re, error_buff, sizeof(error_buff));
      fprintf(stderr, "grep: %s\n", error_buff);
      error = 2;
    }
    regfree(&re);
  }
  return error;
}