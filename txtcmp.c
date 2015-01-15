#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#define PROG "txtcmp"
#define VERSION "0.1.0"

typedef unsigned long hash_t;

static void print_usage(FILE *fp, const char *arg0)
{
  fprintf(fp,
    "Usage: %s file1 [file2...]\n"
    "Options:\n"
    " -h     Print this message\n"
    " -v     Print the version\n"
    , arg0);
}

static hash_t
hash_string(const char* str)
{
  hash_t hash = 0;
  int c;

  // sdbm hash function
  while((c = *str++)) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  // We use a hash of zero to indicate EOF.
  hash = (hash == 0) ? 1 : hash;

  return hash;
}

static void
hash_file(FILE *fp, hash_t **buffer)
{
  char *line = NULL;
  size_t linecap = 0, linecount = 0, lineguess = 100;
  ssize_t linelen;

  if((*buffer = malloc(lineguess * sizeof(hash_t))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }

  errno = 0;
  while((linelen = getline(&line, &linecap, fp)) > 0) {
    ++linecount;

    // Rellocate memory if we have underestimated the file line count.
    if(linecount > lineguess) {
      lineguess *= 2;
      if((*buffer = realloc(*buffer, lineguess * sizeof(hash_t))) == NULL) {
        perror("realloc()");
        exit(EXIT_FAILURE);
      }
    }

    (*buffer)[linecount-1] = hash_string(line);
    errno = 0;
  }
  if(errno != 0) {
    perror("getline()");
    exit(EXIT_FAILURE);
  }

  // Shrink the buffer down to the actual file length.
  if((*buffer = realloc(*buffer, (linecount + 1) * sizeof(hash_t))) == NULL) {
    perror("realloc()");
    exit(EXIT_FAILURE);
  }

  // "Terminate" the sequence of hashes with zero.
  (*buffer)[linecount] = 0;

  if(line != NULL) {
    free(line);
  }
}

static void
hash_files(const char *const* filenames, hash_t **hashes, size_t length)
{
  size_t i;

  for(i = 0; i < length; ++i) {
    FILE *fp = fopen(filenames[i], "r");
    if(fp == NULL) {
      perror("fopen()");
      exit(EXIT_FAILURE);
    }
    hash_file(fp, &hashes[i]);
    fclose(fp);
  }
}

int main(int argc, char **argv)
{
  int ch, count;
  hash_t **hashes;
  char **filenames;

  while((ch = getopt(argc, argv, "hv")) != -1) {
    switch(ch) {
      case 'v':
        printf(PROG " " VERSION "\n");
        exit(EXIT_SUCCESS);
      case 'h':
        print_usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
      case '?':
      default:
        print_usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  count = argc - optind;
  if(count < 2) {
    fprintf(stderr, "%s: need at least two filenames\n", argv[0]);
    print_usage(stderr, argv[0]);
    exit(EXIT_FAILURE);
  }

  if((hashes = malloc(count * sizeof(hash_t*))) == NULL) {
    perror("malloc()");
    exit(EXIT_FAILURE);
  }
  filenames = argv + optind;
  hash_files((const char *const*) filenames, hashes, count);

  free(hashes);
  exit(EXIT_SUCCESS);
}
