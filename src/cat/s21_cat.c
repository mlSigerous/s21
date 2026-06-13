#include "s21_cat.h"

int main(int argc, char* argv[]) {
  Flags flags = {0};
  int error = 0;
  int row = 1;
  int start = 1;
  int empty_rows = 0;
  if (argc > 1) {
    error = process_options(argc, argv, &flags);
    int files_error =
        process_files(argc, argv, &flags, &row, &start, &empty_rows);
    if (!error) {
      error = files_error;
    }
  } else {
    print_file(stdin, &flags, &row, &start, &empty_rows);
  }
  return error;
}