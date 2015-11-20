#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "http.h"
#include "error.h"
#include "request.h"
#include "last_modified.h"

// check if method is right
int checkMethodOfRequest(Request request)
{
  char* method;

  method = stringToLower(request -> startLine -> method);

  if (strcmp(method, "get") != 0 && strcmp(method, "post") != 0)
  {
    hcode = 400;
    return ERROR;
  }

  return SUCCESS;
}

// check if host header is existed and right
int checkHostOfRequest(Request request)
{
  char *host;
  if ((host = getHeaderOfRequest(request, "host")) == NULL)
  {
    hcode = 400;
    return ERROR;
  }

  // if host is wrong, return 400
  if ((strncmp(host, "127.0.0.1", strlen("127.0.0.1")) != 0) && (strncmp(host, "localhost", strlen("localhost")) != 0))
  {
    hcode = 400;
    return ERROR;
  }

  if (strncmp(host, "localhost", strlen("localhost")) == 0)
  {
    hcode = 301;
    return ERROR;
  }

  return SUCCESS;
}

int checkLastModifiedOfFile(Request request)
{
  printf("in checkLastModifiedOfFile\n");

  char path[PATH_LEN], modifiedTimeOfFile[TIME_LEN];
  char *lastModified;

  getPathFromUrl(request -> startLine -> url, path, PATH_LEN);
  if ((lastModified = getHeaderOfRequest(request, "if-modified-since")) == NULL)
  {
    printf("If-Modified-Since is NULL\n");
    return SUCCESS;
  }

  getLastModifiedTime(path, modifiedTimeOfFile, TIME_LEN);

  printf("modifiedTimeOfFile: %s\n", modifiedTimeOfFile);
  printf("lastModified: %s\n", lastModified);

  if (strcmp(modifiedTimeOfFile, lastModified) == 0)
  {
    hcode = 304;
    return ERROR;    
  }

  return SUCCESS;
}