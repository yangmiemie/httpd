#include <stdio.h>
#include <string.h>

#define FILETYPELEN 8
#define CONTENTTYPELEN 16

struct contentType
{
  char* fileType;
  char* contentType;
};

struct contentType fileToContentType[] = {
  {"jpg", "image/jpeg"},
  {"png", "image/png"},
  {"gif", "image/gif"},
  {"html", "text/html"},
  {"js", "application/x-javascript"},
  {"css", "text/css"}
};

char* getContentTypeFromFileType(char* fileType)
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
int getFileTypeFromPath(char* path, char* fileType, int size)
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
    return -1;

  ++i;
  for (j = 0; i < len; ++i, ++j)
    fileType[j] = path[i];

  fileType[j] = '\0';
  return 0;
}

char* getContentTypeFromPath(char* path)
{
  char fileType[FILETYPELEN];

  if (getFileTypeFromPath(path, fileType, FILETYPELEN) < 0)
    return NULL;

  return getContentTypeFromFileType(fileType);
}

int testGetContentTypeFromFileType()
{
  int i, len;

  len = sizeof(fileToContentType) / sizeof(struct contentType);

  for (i = 0; i < len; ++i)
  {
    printf("content type of %s: %s\n", fileToContentType[i].fileType, getContentTypeFromFileType(fileToContentType[i].contentType));
  }

  return 0;
}

void printFileTypeResult(char *path, int result, char *fileType)
{
  if (result == 0)
  {
    printf("fileType of path %s: %s\n", path, fileType);
  }
  else
  {
    printf("path %s returned error\n", path);
  }
}

int testGetFileTypeFromPath()
{
  char *path = "/file/a/1.jpg";
  char fileType[FILETYPELEN];
  int result;

  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  printFileTypeResult(path, result, fileType);

  path = "/file/1.html";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  printFileTypeResult(path, result, fileType);

  path = "/var/httpd/1.png";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  printFileTypeResult(path, result, fileType);

  path = "/var/1.httpd1png";
  result = getFileTypeFromPath(path, fileType, FILETYPELEN);
  printFileTypeResult(path, result, fileType);
}

int testGetContentTypeFromPath()
{
  char *path = "/file/a/1.jpg";
  char Type[FILETYPELEN];
  int result;

  printf("contentType of %s: %s\n", path, getContentTypeFromPath(path));

  path = "/file/1.html";
  printf("contentType of %s: %s\n", path, getContentTypeFromPath(path));

  path = "/var/httpd/1.png";
  printf("contentType of %s: %s\n", path, getContentTypeFromPath(path));
}

#undef FILETYPELEN
#undef CONTENTTYPELEN