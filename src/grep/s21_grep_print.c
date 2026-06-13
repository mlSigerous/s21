#include "s21_grep.h"

void print_flag_o(const Flags* flags, regex_t* re, char* line,
                  const char* filename, int row, int files_cnt) {
  regmatch_t match;
  int offset = 0;
  int done = 0;
  while (!done) {
    int res = regexec(re, line + offset, 1, &match, 0);
    if (res != 0) {
      done = 1;
    } else {
      if (match.rm_eo > match.rm_so) {
        if (filename != NULL && !flags->h && files_cnt > 1) {
          printf("%s:", filename);
        }
        if (flags->n) {
          printf("%d:", row);
        }
        for (int i = match.rm_so; i < match.rm_eo; i++) {
          putchar(line[offset + i]);
        }
        putchar('\n');
      }
      if (match.rm_so == match.rm_eo) {
        if (line[offset] == '\0') {
          done = 1;
        } else {
          offset++;
        }
      } else {
        offset += match.rm_eo;
      }
    }
  }
}

void print_usage(void) {
  fprintf(stderr,
          "Usage: grep [OPTION]... PATTERNS [FILE]...\n"
          "Try 'grep --help' for more information.\n");
}

void print_match(const Flags* flags, regex_t* re_o, int re_o_cnt, char* line,
                 const char* filename, int row, int files_cnt, int* vo_active) {
  int copy_o = flags->o && !flags->v;
  if (copy_o && !flags->c && !flags->l) {
    int printed = 0;
    for (int i = re_o_cnt - 1; i >= 0 && !printed; i--) {
      if (regexec(&re_o[i], line, 0, NULL, 0) == 0) {
        print_flag_o(flags, &re_o[i], line, filename, row, files_cnt);
        printed = 1;
      }
    }
  } else if (flags->o && flags->v && !flags->c && !flags->l) {
    printf("%d:", row);
    *vo_active = 1;
  } else if (!flags->c && !flags->l && !(flags->v && flags->o)) {
    if (files_cnt > 1 && !flags->h && filename != NULL) {
      printf("%s:", filename);
    }
    if (flags->n) {
      printf("%d:", row);
    }
    printf("%s\n", line);
  }
}

void print_cnt(const Flags* flags, const char* filename, int files_cnt,
               int match_cnt) {
  if (flags->c && flags->l) {
    if (files_cnt > 1) {
      if (match_cnt > 0) {
        printf("%s\n", filename ? filename : "(standard input)");
      } else if (flags->h) {
        printf("0\n");
      } else {
        printf("%s:0\n", filename ? filename : "(standard input)");
      }
    } else if (match_cnt > 0) {
      printf("%s\n", filename ? filename : "(standard input)");
    } else {
      printf("0\n");
    }
  } else if (flags->l) {
    if (match_cnt > 0) {
      printf("%s\n", filename ? filename : "(standard input)");
    }
  } else if (flags->c) {
    if (files_cnt > 1 && !flags->h && filename != NULL) {
      printf("%s:%d\n", filename, match_cnt);
    } else {
      printf("%d\n", match_cnt);
    }
  }
}

int process_line(char* line, Flags* flags, PatternsList* patterns, int* error) {
  int match = match_patterns(line, patterns, flags, error);
  if (!*error && flags->v) {
    match = !match;
  }
  return match;
}

void trim_newline(char* line) {
  size_t len = strlen(line);
  if (len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0';
  }
}

int print_regexp(FILE* fp, Flags* flags, const char* filename, int files_cnt,
                 PatternsList* patterns, regex_t* re_o, int re_o_cnt,
                 int* error) {
  char* line = NULL;
  size_t capacity = 0;
  int row = 1;
  int match_cnt = 0;
  int vo_active = 0;
  while (getline(&line, &capacity, fp) != -1 && !*error) {
    trim_newline(line);
    int match = process_line(line, flags, patterns, error);
    if (!*error && match) {
      match_cnt++;
      print_match(flags, re_o, re_o_cnt, line, filename, row, files_cnt,
                  &vo_active);
    }
    row++;
  }
  if (vo_active) {
    putchar('\n');
  }
  if (ferror(fp)) {
    if (!flags->s) {
      fprintf(stderr, "grep: %s: %s\n",
              filename != NULL ? filename : "(standard input)",
              strerror(errno));
    }
    *error = 2;
  }
  if (!*error) {
    print_cnt(flags, filename, files_cnt, match_cnt);
  }
  free(line);
  return match_cnt > 0;
}

void process_files(int argc, char* argv[], int files_cnt, Flags* flags,
                   PatternsList* patterns, regex_t* re_o, int re_o_cnt,
                   int* found, int* error, int* file_error) {
  for (int i = optind; i < argc; i++) {
    FILE* fp = strcmp(argv[i], "-") == 0 ? stdin : fopen(argv[i], "r");
    if (fp != NULL) {
      if (print_regexp(fp, flags, argv[i], files_cnt, patterns, re_o, re_o_cnt,
                       error)) {
        *found = 1;
      }
      if (fp != stdin) {
        fclose(fp);
      }
    } else {
      *file_error = 1;
      if (!flags->s) {
        fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
      }
    }
  }
}