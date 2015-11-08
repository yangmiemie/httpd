#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "http.h"
#include "error.h"
#include "request.h"

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
  if ((host = getHostOfRequest(request)) == NULL)
  {
    hcode = 400;
    return ERROR;
  }

  // if host is wrong, return 400
  if ((strcmp(host, "127.0.0.1") != 0) && (strcmp(host, "localhost") != 0))
  {
    hcode = 400;
    return ERROR;
  }

  if (strcmp(host, "localhost") == 0)
  {
    hcode = 301;
    return ERROR;
  }

  return SUCCESS;
}
