#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "http.h"

char* stringToLower(char* string)
{
  int i;

  for (i = 0; string[i] != '\0'; ++i)
  {
    string[i] = tolower(string[i]);
  }

  return string;
}

void getPathFromUrl(char *url, char *path, int size)
{
  int i, j;

  memset(path, 0, size);
  sprintf(path, "%s", FILE_DIRECTORY);

  j = strlen(path);
  for (i = 0; url[i] != '\0' && url[i] != '?' && i < size; ++i, ++j)
    path[j] = url[i];

  printf("path in getPathFromUrl: %s\n", path);
  printf("last char of path: %c\n", path[strlen(path) - 1]);

  if (path[strlen(path) - 1] == '/')
    strcat(path, "index.html");
}