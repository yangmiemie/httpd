#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "../src/error.h"
#include "../src/content_length.h"

void testGetContentLengthFromFile()
{
  char *file = "/var/httpd/index.html";
  struct stat sbuf;

  stat(file, &sbuf);
  assert(getContentLengthFromFile(file) == sbuf.st_size);

  file = "/var/httpd/1.jpg";
  stat(file, &sbuf);
  assert(getContentLengthFromFile(file) == sbuf.st_size);
  
  file = "/var/httpd/not_exist.jpg";
  assert(getContentLengthFromFile(file) == ERROR);
}
