#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

// get content length of response, which is the size of the file
int getContentLengthFromFile(char* file)
{
  struct stat statbuf;

  if (stat(file, &statbuf) < 0)
    return -1;

  return statbuf.st_size;
}

void testGetContentLengthFromFile()
{
  char *file = "/var/httpd/index.html";

  printf("content length of %s: %d\n", file, getContentLengthFromFile(file));

  file = "/var/httpd/1.jpg";
  printf("content length of %s: %d\n", file, getContentLengthFromFile(file));  

  file = "/var/httpd/12.jpg";
  printf("content length of %s: %d\n", file, getContentLengthFromFile(file));  
}
