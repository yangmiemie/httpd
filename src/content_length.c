#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "error.h"

// get content length of response, which is the size of the file
int getContentLengthFromFile(char* file)
{
  struct stat statbuf;

  if (stat(file, &statbuf) < 0)
    return ERROR;

  return statbuf.st_size;
}
