#define PROG "txtcmp"
#define VERSION "0.2.1"

#ifndef TXTCMP_LENGTH_GUESS
# define TXTCMP_LENGTH_GUESS 100
#endif
#if TXTCMP_LENGTH_GUESS < 1
# error "TXTCMP_LENGTH_GUESS should be >= 1"
#endif

#ifndef TXTCMP_REALLOC_FACTOR
# define TXTCMP_REALLOC_FACTOR 2
#endif
#if TXTCMP_REALLOC_FACTOR < 2
# error "TXTCMP_REALLOC_FACTOR should be >= 2"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

typedef unsigned long hash_t;

// Options set by command line flags.
static char opt_normalize = 0;
static char opt_trim = 0;
static char opt_ignore_blanks = 0;
static char opt_ignore_whitespace = 0;

static void
xperror(const char *msg)
{
  if(msg == NULL) {
    perror(PROG);
  }
  else {
    fprintf(stderr, PROG ": ");
    perror(msg);
  }
}

static void print_usage(FILE *fp, const char *arg0)
{
  fprintf(fp,
    "Usage: %s [options] file1 [file2...]\n"
    "Options:\n"
    " -b     Ignore blank lines\n"
    " -s     Ignore whitespace altogether\n"
    " -t     Trim whitespace from ends of lines\n"
    " -n     Normalize LCS lengths\n"
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
hash_file(const char *filename, FILE *fp, hash_t **buffer)
{
  char *line = NULL, *linestart = NULL;
  size_t linecap = 0, linecount = 0, lineguess = TXTCMP_LENGTH_GUESS;
  ssize_t linelen;

  if((*buffer = malloc(lineguess * sizeof(**buffer))) == NULL) {
    xperror("malloc()");
    exit(EXIT_FAILURE);
  }

  while((linelen = getline(&line, &linecap, fp)) > 0) {
    ++linecount;
    linestart = line;

    // Trim whitespace
    if(opt_trim) {
      while(isspace(*linestart)) {
        ++linestart;
        --linelen;
      }
      while(isspace(linestart[linelen - 1])) {
        --linelen;
      }
      linestart[linelen] = '\0';
    }

    // Strip all whitespace in-place.
    if(opt_ignore_whitespace) {
      size_t i, imax = linelen;
      char *insert = linestart;

      for(i = 0; i < imax; ++i) {
        if(!isspace(linestart[i])) {
          *insert = linestart[i];
          ++insert;
        }
        else {
          --linelen;
        }
      }

      linestart[linelen] = '\0';
    }

    // Ignore the line if it is blank
    if(opt_ignore_blanks && (
        (linestart[0] == '\r' && linestart[1] == '\n') ||
        linestart[0] == '\n' ||
        linestart[0] == '\0'
      )
    ) {
      --linecount;
      continue;
    }

    // Rellocate memory if we have underestimated the file line count.
    if(linecount > lineguess) {
      lineguess *= TXTCMP_REALLOC_FACTOR;
      if((*buffer = realloc(*buffer, lineguess * sizeof(**buffer))) == NULL) {
        xperror("realloc()");
        exit(EXIT_FAILURE);
      }
    }

    (*buffer)[linecount-1] = hash_string(linestart);
  }
  if(ferror(fp)) {
    xperror(filename);
    exit(EXIT_FAILURE);
  }

  // Shrink the buffer down to the actual file length.
  if((*buffer = realloc(*buffer, (linecount + 1) * sizeof(**buffer))) == NULL) {
    xperror("realloc()");
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
  FILE *fp = NULL;

  for(i = 0; i < length; ++i) {
    if((fp = fopen(filenames[i], "r")) == NULL) {
      xperror(filenames[i]) ;
      exit(EXIT_FAILURE);
    }

    hash_file(filenames[i], fp, &hashes[i]);
    fclose(fp);
  }
}

unsigned long
lcs_length(hash_t *buf1, size_t buflen1, hash_t *buf2, size_t buflen2)
{
  size_t i, j;
  unsigned long *swap, *this_row, *last_row, lookup1, lookup2, result;
  hash_t h1, h2;

  if(buflen1 == 0 || buflen2 == 0) {
    return 0;
  }

  // Make sure buf2 is the smaller buffer (to allocate less memory).
  if(buflen2 > buflen1) {
    size_t tmplen;
    hash_t *tmpbuf;

    tmpbuf = buf1;
    tmplen = buflen1;
    buf1 = buf2;
    buflen1 = buflen2;
    buf2 = tmpbuf;
    buflen2 = tmplen;
  }

  if((this_row = malloc(buflen2 * sizeof(*this_row))) == NULL) {
    xperror("malloc()");
    exit(EXIT_FAILURE);
  }

  if((last_row = malloc(buflen2 * sizeof(*last_row))) == NULL) {
    xperror("malloc()");
    exit(EXIT_FAILURE);
  }

  for(i = 0; i < buflen1; i++) {
    h1 = buf1[i];
    for(j = 0; j < buflen2; j++) {
      h2 = buf2[j];

      if(h1 == h2) {
        lookup1 = 0;
        if(i > 0 && j > 0) {
          lookup1 = last_row[j - 1];
        }
        this_row[j] = 1 + lookup1;
      }
      else {
        lookup1 = 0;
        lookup2 = 0;
        if(i > 0) {
          lookup1 = last_row[j];
        }
        if(j > 0) {
          lookup2 = this_row[j - 1];
        }
        this_row[j] = (lookup1 > lookup2) ? lookup1 : lookup2;
      }
    }
    swap = this_row;
    this_row = last_row;
    last_row = swap;
  }

  result = last_row[buflen2 - 1];
  free(this_row);
  free(last_row);
  return result;
}

size_t hashlen(hash_t *hash) {
  size_t i = 0;
  while(hash[i] != 0) {
    ++i;
  }
  return i;
}

int main(int argc, char **argv)
{
  size_t i, j, len1, len2;
  unsigned long length;
  int ch, count;
  hash_t **hashes;
  char **filenames;

  while((ch = getopt(argc, argv, "bstnhv")) != -1) {
    switch(ch) {
      case 'v':
        printf(PROG " " VERSION "\n");
        exit(EXIT_SUCCESS);
      case 'h':
        print_usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
      case 'b':
        opt_ignore_blanks = 1;
        continue;
      case 's':
        opt_ignore_whitespace = 1;
        continue;
      case 'n':
        opt_normalize = 1;
        continue;
      case 't':
        opt_trim = 1;
        continue;
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

  if((hashes = malloc(count * sizeof(*hashes))) == NULL) {
    xperror("malloc()");
    exit(EXIT_FAILURE);
  }
  filenames = argv + optind;
  hash_files((const char *const*) filenames, hashes, count);

  for(i = 0; i < count; i++) {
    len1 = hashlen(hashes[i]);
    for(j = i + 1; j < count; j++) {
      len2 = hashlen(hashes[j]);
      length = lcs_length(hashes[i], len1, hashes[j], len2);

      if(!opt_normalize) {
        printf("%lu %s %s\n", length, filenames[i], filenames[j]);
      }
      else {
        float rank = 0.f;
        size_t minlen;

        minlen = (len1 < len2) ? len1 : len2;
        if(minlen > 0) {
          rank = length / ((float)  minlen);
        }

        printf("%.3f %s %s\n", rank, filenames[i], filenames[j]);
      }
    }
  }

  for(i = 0; i < count; i++) {
    free(hashes[i]);
  }
  free(hashes);
  exit(EXIT_SUCCESS);
}
