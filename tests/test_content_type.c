#include <assert.h>

#include "../src/error.h"
#include "../src/content_type.h"

extern struct contentType fileToContentType[];

void testGetFileTypeFromPath()
{
  char *path = "/file/a/1.jpg";
  char fileType[FILETYPELEN];
  int result;

  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  assert(result == SUCCESS);
  assert(strcmp(fileType, "jpg") == 0);

  path = "/file/1.html";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  assert(result == SUCCESS);
  assert(strcmp(fileType, "html") == 0);

  path = "/var/httpd/1.png";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  assert(result == SUCCESS);
  assert(strcmp(fileType, "png") == 0);

  path = "/var/1.httpd1png";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  assert(result == ERROR);
}

void testGetContentTypeFromPath()
{
  char *path = "/file/a/1.jpg";
  assert(strcmp(getContentTypeFromPath(path), "image/jpeg") == 0);

  path = "/file/1.html";
  assert(strcmp(getContentTypeFromPath(path), "text/html") == 0);

  path = "/var/httpd/1.png";
  assert(strcmp(getContentTypeFromPath(path), "image/png") == 0);
}

void testGetContentTypeFromFileType()
{
  int i, len;

  // len = sizeof(fileToContentType) / sizeof(struct contentType);
  len = 5;

  for (i = 0; i < len; ++i)
  {
    assert(strcmp(getContentTypeFromFileType(fileToContentType[i].fileType), fileToContentType[i].contentType) == 0);
  }
}