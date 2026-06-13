#include "s21_cat.h"

int is_flag(const char* arg) {
  int flag = 0;
  if ((arg[0] == '-') && (strlen(arg) > 1)) {
    flag = 1;
  }
  return flag;
}

int is_file(const char* arg) {
  int flag = 0;
  if (arg[0] != '-') {
    flag = 1;
  }
  return flag;
}

int is_long(const char* arg) {
  int flag = 0;
  if (arg[0] == '-' && arg[1] == '-') {
    flag = 1;
  }
  return flag;
}

int parse_long_flags(const char* arg, Flags* flags) {
  int error = 0;
  if (strcmp(arg, "--number-nonblank") == 0) {
    flags->b = 1;
  } else if (strcmp(arg, "--number") == 0) {
    flags->n = 1;
  } else if (strcmp(arg, "--squeeze-blank") == 0) {
    flags->s = 1;
  } else {
    error = 1;
  }
  return error;
}

int parse_short_flags(const char* arg, Flags* flags) {
  int error = 0;
  int len = strlen(arg);
  for (int i = 0; i < len && !error; i++) {
    int found = 0;
    if (arg[i] == 'b') {
      flags->b = 1;
      found = 1;
    }
    if (arg[i] == 'n') {
      flags->n = 1;
      found = 1;
    }
    if (arg[i] == 's') {
      flags->s = 1;
      found = 1;
    }
    if (arg[i] == 'v') {
      flags->v = 1;
      found = 1;
    }
    if (arg[i] == 'e') {
      flags->v = 1;
      flags->e = 1;
      found = 1;
    }
    if (arg[i] == 't') {
      flags->v = 1;
      flags->t = 1;
      found = 1;
    }
    if (arg[i] == 'T') {
      flags->t = 1;
      found = 1;
    }
    if (arg[i] == 'E') {
      flags->e = 1;
      found = 1;
    }
    if (!found) {
      error = arg[i];
    }
  }
  return error;
}

int parse_flags(const char* arg, Flags* flags) {
  int check = is_long(arg);
  int error = 0;
  if (check) {
    error = parse_long_flags(arg, flags);
  } else {
    error = parse_short_flags(arg + 1, flags);
  }
  return error;
}

int process_options(int argc, char* argv[], Flags* flags) {
  int error = 0;
  for (int i = 1; i < argc && !error; i++) {
    if (is_flag(argv[i])) {
      int flags_error = parse_flags(argv[i], flags);
      if (flags_error) {
        if (is_long(argv[i])) {
          fprintf(stderr,
                  "cat: invalid option -- '%s'\n"
                  "Try 'cat --help' for more information.",
                  argv[i]);
        } else {
          fprintf(stderr,
                  "cat: invalid option -- '%c'\n"
                  "Try 'cat --help' for more information.",
                  flags_error);
        }
        error = 1;
      }
    }
  }
  return error;
}

int process_files(int argc, char* argv[], const Flags* flags, int* row,
                  int* start, int* empty_rows) {
  int error = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      print_file(stdin, flags, row, start, empty_rows);
    } else if (is_file(argv[i])) {
      FILE* fp = fopen(argv[i], "r");
      if (fp != NULL) {
        print_file(fp, flags, row, start, empty_rows);
        fclose(fp);
      } else {
        fprintf(stderr, "%s: no such file or directory\n", argv[i]);
        error = 1;
      }
    }
  }
  return error;
}