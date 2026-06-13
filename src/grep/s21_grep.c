#include "s21_grep.h"

int main(int argc, char* argv[]) {
  Flags flags = {0};
  PatternsList patterns;
  regex_t* re_o = NULL;
  int re_o_cnt = 0;
  int re_o_comp = 0;
  int error = 0;
  int found = 0;
  int file_error = 0;
  int exit_code = 1;
  init_patterns(&patterns);
  error = parse_flags(argc, argv, &flags, &patterns);
  if (!error && !flags.e && !flags.f && patterns.cnt == 0 && optind < argc) {
    error = add_pattern(argv[optind++], &patterns);
  }
  if (!error) {
    error = validate_patterns(&patterns, &flags);
  }
  if (!error && patterns.cnt == 0) {
    print_usage();
    error = 2;
  }
  if (!error && flags.o) {
    error = compile_patterns_o(&patterns, &flags, &re_o, &re_o_cnt);
    if (!error) {
      re_o_comp = 1;
    }
  }
  if (!error) {
    if (optind < argc) {
      process_files(argc, argv, argc - optind, &flags, &patterns, re_o,
                    re_o_cnt, &found, &error, &file_error);
    } else {
      if (print_regexp(stdin, &flags, NULL, 1, &patterns, re_o, re_o_cnt,
                       &error)) {
        found = 1;
      }
    }
  }
  free_patterns(&patterns);
  if (re_o_comp) {
    free_patterns_o(re_o, re_o_cnt);
  }
  if (error || file_error) {
    exit_code = 2;
  } else if (found) {
    exit_code = 0;
  }
  return exit_code;
}