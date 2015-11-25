#include <stdio.h>
#include <string.h>

#include "error.h"
#include "content_type.h"

struct contentType fileToContentType[] = {
  {"jpg", "image/jpeg"},
  {"png", "image/png"},
  {"gif", "image/gif"},
  {"html", "text/html"},
  {"txt", "text/plain"},
  {"js", "application/x-javascript"},
  {"css", "text/css"},
  {"webp", "image/webp"},
  {"xhtml", "application/xhtml+xml"}
};

char* getContentTypeFromFileType(const char* fileType)
{
  int i, len;

  len = sizeof(fileToContentType) / sizeof(struct contentType);

  for (i = 0; i < len; ++i)
  {
    if (strcmp(fileToContentType[i].fileType, fileType) == 0)
    {
      return fileToContentType[i].contentType;
    }
  }

  // default request regard as a html file
  return "text/html";
}

// The argument path must be a string and end up with '\0'.
// The resulting string is copied to the buffer pointed by fileType.
// The number of bytes available in this buffer is the argument size.
int getFileTypeFromPath(const char* path, char* fileType, int size)
{
  int i, j, len;

  len = strlen(path);

  for (i = len - 1; i >= 0; --i)
  {
    if (path[i] == '.')
      break;
  }

  // The size of fileType is greater than the bytes available in buffer.
  if (strlen(&path[i]) > size - 1)
    return ERROR;

  ++i;
  for (j = 0; i < len; ++i, ++j)
    fileType[j] = path[i];

  fileType[j] = '\0';

  return SUCCESS;
}

char* getContentTypeFromPath(const char* path)
{
  char fileType[FILETYPELEN];

  if (getFileTypeFromPath(path, fileType, FILETYPELEN) < 0)
    return NULL;

  return getContentTypeFromFileType(fileType);
}