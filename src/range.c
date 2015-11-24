#include "range.h"
#include "error.h"
#include "http.h"

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// if Range value , such as Range: abc, is invalid, then ignore it and return Success.
// format of range value: bytes=m-n, bytes=m- (from mth byte to last), bytes=-n (last n bytes).
// if format of bytes if invalid, then return Error. Code is 416, Requested Range Not Satisfiable
// if offset return -1, then server return last len bytes.
int getRangeOfRequest(char* range, char *path, int* offset, int* len)
{
  printf("in getRangeOfRequest\n");
  char *ptrFront, *ptrBack;
  char buf[HEADER_VALUE_LEN];
  char *first, *second;
  int size;
  struct stat sbuf;

  printf("path: %s\n", path);
  if (stat(path, &sbuf) < 0)
  {
    perror("stat error");
    return ERROR;
  }

  size = sbuf.st_size;

  memset(buf, 0, HEADER_VALUE_LEN);
  strcpy(buf, range);
  printf("buf:%s\n", buf);

  ptrFront = buf;

  for ( ; *ptrFront == ' '; ++ptrFront);

  ptrBack = ptrFront;

  for ( ; *ptrFront != '='; ++ptrFront);

  *ptrFront = '\0';

  printf("ptrFront: %s\n", ptrFront);
  printf("ptrBack: %s\n", ptrBack);
  // format is invalid and return 
  if (strcmp(ptrBack, "bytes") != 0)
  {
    return SUCCESS;
  }

  ++ptrFront;
  for ( ; *ptrFront == ' '; ++ptrFront);

  ptrBack = ptrFront;

  for ( ; *ptrFront != '-'; ++ptrFront);

  printf("ptrFront: %s\n", ptrFront);
  printf("ptrBack: %s\n", ptrBack);

  *ptrFront = '\0';
  first = ptrBack;

  ++ptrFront;
  second = ptrFront;

  printf("first: %s\n", first);
  printf("second: %s\n", second);

  if (checkCharOfString(first) < 0 || checkCharOfString(second) < 0)
  {
    printf("not char\n");
    return ERROR;
  }

  // format of bytes:-
  if ((strcmp(first, "") == 0) && (strcmp(second, "") == 0))
  {
    printf("both emtpy\n");
    return ERROR;
  }

  // get last n bytes
  if (strcmp(first, "") == 0)
  {
    printf("first emtpy\n");
    *len = atoi(second);
    *offset = (size - *len);

    printf("offset: %d\n", *offset);
    printf("len: %d\n", *len);

    return SUCCESS;
  }

  // get bytes from nth byte to last
  if (strcmp(second, "") == 0)
  {
    printf("second emtpy\n");
    *offset = atoi(first);
    *len = (size - *offset);

    printf("offset: %d\n", *offset);
    printf("len: %d\n", *len);

    return SUCCESS;
  }

  printf("all not emtpy\n");

  *offset = atoi(first);
  *len = atoi(second) - *offset + 1;

  printf("offset: %d\n", *offset);
  printf("len: %d\n", *len);
  return SUCCESS;
}

int checkCharOfString(char* buf)
{
  char *ptr;
  int c;

  for (c = 0, ptr = buf; *ptr != '\0'; ++ptr)
    if ((c = *ptr) && isdigit(c) == 0)
    {
      return ERROR;      
    }

  return SUCCESS;
}
