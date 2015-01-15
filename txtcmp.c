#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define PROG "txtcmp"
#define VERSION "0.1.0"

static void print_usage(FILE *fp, const char *arg0)
{
  fprintf(fp,
    "Usage: %s file1 [file2...]\n"
    "Options:\n"
    " -h     Print this message\n"
    " -v     Print the version\n"
    , arg0);
}

int main(int argc, char **argv)
{
  int ch;

  while((ch = getopt(argc, argv, "hv")) != -1) {
    switch(ch) {
      case 'v':
        printf(PROG " " VERSION "\n");
        exit(EXIT_SUCCESS);
      case 'h':
        print_usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
      default:
        print_usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}
