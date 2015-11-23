#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "http.h"
#include "request.h"
#include "error.h"
#include "utils.h"

// return -1 means read error
int acceptRequest(int sockfd, Request request)
{
  int n, i, headerSize;
  char buf[HEADER_NAME_LEN + HEADER_VALUE_LEN];
  headerSize = HEADER_VALUE_LEN + HEADER_NAME_LEN;

  // the first line of request is start line
  if ((n = readLineFromSocket(sockfd, buf, headerSize)) > 0)
  {
    if ((n = parseStartLineOfRequest(buf, n, request -> startLine)) < 0)
    {
      hcode = BAD_REQUEST;
      return ERROR;
    }
  }
  else
  {
    // get FIN or Error from client
    printf("readLineFromSocket get n: %d\n", n);
    return n;
  }

  i = 0;

  while ((n = readLineFromSocket(sockfd, buf, headerSize)) > 0)
  {
    (request -> headers)[i] = malloc(sizeof(struct header));
    if (parseHeaderOfRequest(buf, n, (request -> headers)[i]) < 0)
      {
        hcode = BAD_REQUEST;
        return ERROR;
      }

    ++i;
    request -> headersNumber = i;
  }

  return SUCCESS;
}

int parseHeaderOfRequest(char *buf, int size, Header header)
{
  int i, j;

  i = 0;
  if ((i = skipSpaceAndCheckEnd(buf, i, size)) < 0)
  {
    hcode = BAD_REQUEST;
    return ERROR;
  }

  for (j = 0; i < size && !ISCOLON(buf[i]); ++i, ++j)
    header -> name[j] = buf[i];
  header -> name[j] = '\0';
  
  // skip colon
  ++i;

  if ((i = skipSpaceAndCheckEnd(buf, i, size)) < 0)
  {
    hcode = BAD_REQUEST;
    return ERROR;
  }

  for (j = 0; i < size && !ISNEWLINE(buf[i]); ++i, ++j)
    header -> value[j] = buf[i];
  header -> value[j] = '\0';
  
  return SUCCESS;
}

int skipSpaceAndCheckEnd(char* buf, int i, int n)
{
  while (ISSPACE(buf[i]) && (buf[i] != '\0'))
    ++i;

  if (i >= n)
  {
    hcode = BAD_REQUEST;
    return ERROR;
  }

  return i;  
}

// get start line of request and parse into method, url and http version
// return -1 when start line error
int parseStartLineOfRequest(char* buf, int size, RequestStartLine requestStartLine)
{
  int i, j;

  i = 0;
  if ((i = skipSpaceAndCheckEnd(buf, i, size)) < 0)
  {
    hcode = BAD_REQUEST;    
    return ERROR;
  }

  for (j = 0; i < size && !ISSPACE(buf[i]); ++i, ++j)
    requestStartLine -> method[j] = buf[i];
  requestStartLine -> method[j] = '\0';

  if ((i = skipSpaceAndCheckEnd(buf, i, size)) < 0)
  {
    hcode = BAD_REQUEST;
    return ERROR;
  }

  for (j = 0; i < size && !ISSPACE(buf[i]); ++i, ++j)
    requestStartLine -> url[j] = buf[i];
  requestStartLine -> url[j] = '\0';

  if ((i = skipSpaceAndCheckEnd(buf, i, size)) < 0)
  {
    hcode = BAD_REQUEST;
    return ERROR;
  }

  for (j = 0; i < size && !ISSPACE(buf[i]); ++i, ++j)
    requestStartLine -> httpVersion[j] = buf[i];
  requestStartLine -> httpVersion[j] = '\0';
  
  return SUCCESS;
}

// get bytes from socket, return -1 when read \r\n and return normal character when not.
int readByteFromSocket(int sockfd)
{
  int c, n;
  c = 0;

  if ((n = recv(sockfd, &c, 1, 0)) <= 0)
  {
    return n;
  }

  if (c == '\r')
  {
    if ((n = recv(sockfd, &c, 1, MSG_PEEK)) < 0)
    {
      return n;
    }

    if (c == '\n')
    {
      recv(sockfd, &c, 1, 0);
    }
    
    return -1;
  }

  return c;
}

// read line from socket, return -1 when read \r\n, which means the end of request header.
int readLineFromSocket(int sockfd, char* line, int size)
{
  int c, i, n;

  i = 0;
  c = 0;
  while (i != size - 1)
  {
    if ((c = readByteFromSocket(sockfd)) <= 0)
    {
      if (i == 0)
      {
        // if first char is FIN, then return FIN_CODE, else return error
        if (c == 0)
          return FIN_CODE;
        else
          return ERROR;
      }
      else
        break;
    }

    line[i++] = c;
  }

  line[i] = '\0';
  return i;
}

char* getHeaderOfRequest(Request request, char *header)
{
  int i;

  for (i = 0; i < request -> headersNumber; ++i)
  {
    if (strcmp(stringToLower(((request -> headers)[i]) -> name), header) == 0)
      return (request -> headers[i]) -> value;
  }
  
  return NULL;
}

Request newRequest()
{
  Request request;

  request = malloc(sizeof(struct request));
  request -> startLine = malloc(sizeof(struct requestStartLine));
  memset(request -> startLine, 0, sizeof(struct requestStartLine));

  request -> headersNumber = 0;
  request -> headers = malloc(sizeof(Header) * MAX_HEADERS_NUMBER);
  memset(request -> headers, 0, sizeof(Header) * MAX_HEADERS_NUMBER);

  request -> body = NULL;
  request -> bodySize = 0;

  return request;
}

void freeRequest(Request request)
{
  int i;

  if (request == NULL)
    return;

  if (request -> startLine != NULL)
    free(request -> startLine);

  for (i = 0; i < request -> headersNumber; ++i)
    if ((request -> headers)[i] != NULL)
      free (request -> headers[i]);

  if (request -> body != NULL)
    free(request -> body);

  free(request);
}

void printRequest(Request request)
{
  int i;

  printf("method: %s\n", request -> startLine -> method);
  printf("url: %s\n", request -> startLine -> url);
  printf("version: %s\n", request -> startLine -> httpVersion);

  printf("headers:\n");
  for (i = 0; i < request -> headersNumber; ++i)
  {
    printf("%s:%s\n", request -> headers[i] -> name, request -> headers[i] -> value);
  }
}
