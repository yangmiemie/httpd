#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/error.h"
#include "../src/range.h"

void testRangeOfRequest()
{
  struct stat sbuf;
  int size, n, offset, len;
  char *path = "./test_range.c";
  char *range;

  if (stat(path, &sbuf) < 0)
  {
    perror("stat error");
    exit(1);
  }

  size = sbuf.st_size;

  offset = len = 0;
  range = "bytes=0-10";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == SUCCESS);
  assert(offset == 0);
  assert(len == 11);

  offset = len = 0;
  range = "bytes=0-10";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == SUCCESS);
  assert(offset == 0);
  assert(len == 11);

  offset = len = 0;
  range = "bytes=10-";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == SUCCESS);
  assert(offset == 10);
  assert(len == size - 10);

  offset = len = 0;
  range = "bytes=-10";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == SUCCESS);
  assert(offset == size - 10);
  assert(len == 10);

  offset = len = 0;
  range = "bytes=-";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == ERROR);

  offset = len = 0;
  range = "bytes=12a-12a";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == ERROR);

  offset = len = 0;
  range = "bytes=12a-";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == ERROR);

  offset = len = 0;
  range = "bytes=-12a";
  n = getRangeOfRequest(range, path, &offset, &len);
  assert(n == ERROR);
}