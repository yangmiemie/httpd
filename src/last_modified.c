#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "error.h"

int getLastModifiedTime(const char* file, char* time, int size)
{
  struct stat buf;
  int len;
  struct tm *ptrTime;

  if (stat(file, &buf) < 0)
  {
    return ERROR;
  }

  ptrTime = gmtime(&(buf.st_mtime));

  strftime(time, size, "%a, %d %b %G %T GMT", ptrTime);
  return SUCCESS;
}

// int main(int argc, char const *argv[])
// {
//   char buf[64];

//   getLastModifiedTime("/var/httpd/index.html", buf, 64);
//   printf("len: %d\n", strlen(buf));
//   printf("%s\n", buf);
//   // time_t t;
//   // struct tm tm;
//   // struct tm *tmp;
//   // char buf[64];

//   // memset(buf, 0, 64);
//   // time(&t);
//   // tmp = gmtime(&t);

//   // // Fri, 06 Nov 2015 08:01:39 GMT
//   // printf("%s", asctime(tmp));
//   // strftime(buf, 64, "%a, %d %b %G %T GMT", tmp);
//   // printf("%s\n", buf);
//   return 0;
// }