#include "user.h"

#define MAX_LINES 1000
#define MAX_LINE_LEN 256

int flag_reverse = 0;
int flag_numeric = 0;
int flag_unique = 0;

char *lines[MAX_LINES];
int num_lines = 0;

int
compare_lines(const char *a, const char *b)
{
  int result;

  if (flag_numeric) {
    int num_a = atoi(a);
    int num_b = atoi(b);
    if (num_a < num_b) result = -1;
    else if (num_a > num_b) result = 1;
    else result = strcmp(a, b);
  } else {
    result = strcmp(a, b);
  }

  if (flag_reverse) {
    return -result;
  }
  return result;
}

void
selectionSort(char *arr[], int n)
{
  int i, j, min_idx;
  char *temp;

  for (i = 0; i < n-1; i++) {
    min_idx = i;
    for (j = i+1; j < n; j++) {
      if (compare_lines(arr[j], arr[min_idx]) < 0)
        min_idx = j;
    }
    temp = arr[i];
    arr[i] = arr[min_idx];
    arr[min_idx] = temp;
  }
}


void
sort_file(int fd)
{
  char buffer[MAX_LINE_LEN];
  int i, j;
  int n;
  char c;

  num_lines = 0;

  i = 0;
  while ((n = read(fd, &c, 1)) > 0) {
    if (c == '\n') {
      buffer[i] = '\0';

      if (num_lines < MAX_LINES) {
        lines[num_lines] = malloc(strlen(buffer) + 1);
        if (lines[num_lines] == 0) {
          printf(2, "sort: malloc failed\n");
          exit();
        }
        strcpy(lines[num_lines], buffer);
        num_lines++;
      } else {
        printf(2, "sort: too many lines, max %d\n", MAX_LINES);
        exit();
      }
      i = 0;
    } else if (i < MAX_LINE_LEN - 1) {
      buffer[i++] = c;
    }
  }

  if (i > 0) {
    buffer[i] = '\0';
    if (num_lines < MAX_LINES) {
        lines[num_lines] = malloc(strlen(buffer) + 1);
        if (lines[num_lines] == 0) {
          printf(2, "sort: malloc failed\n");
          exit();
        }
        strcpy(lines[num_lines], buffer);
        num_lines++;
    } else {
      printf(2, "sort: too many lines, max %d\n", MAX_LINES);
      exit();
    }
  }

  selectionSort(lines, num_lines);

  for (j = 0; j < num_lines; j++) {
    if (flag_unique && j > 0 && strcmp(lines[j], lines[j-1]) == 0) continue;
    printf(1, "%s\n", lines[j]);
  }

  for (j = 0; j < num_lines; j++) {
    free(lines[j]);
  }
}

int
main(int argc, char *argv[])
{
  int fd = 0;
  char *filename = 0;
  int i;

  if (argc < 2) {
    printf(2, "usage: sort <filename>\n");
    exit();
  }

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-r") == 0) {
      flag_reverse = 1;
    } else if (strcmp(argv[i], "-n") == 0) {
      flag_numeric = 1;
    } else if (strcmp(argv[i], "-u") == 0) {
      flag_unique = 1;
    } else if (argv[i][0] == '-') {
      printf(2, "sort: unknown flag %s\n", argv[i]);
      exit();
    } else {
      if (filename != 0) {
        printf(2, "sort: too many files, only one is supported\n");
        exit();
      }
      filename = argv[i];
    }
  }

  if (filename) {
    if ((fd = open(filename, 0)) < 0) {
      printf(2, "sort: cannot open %s\n", filename);
      exit();
    }
  }

  sort_file(fd);

  if (fd != 0) {
    close(fd);
  }

  exit();
}