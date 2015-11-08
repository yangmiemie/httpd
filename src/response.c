#include "http.h"
#include "response.h"
#include "error.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

struct codeToDescription
{
  int code;
  char* description;
};

struct codeToBody
{
  int code;
  char *body;
};

struct codeToDescription theCodeToDescription[] = {
  {200, "OK"},
  {201, "Created"},
  {202, "Accepted"},
  {203, "Non-Authoritative Information"},
  {204, "No Content"},
  {301, "Moved Permanently"},
  {302, "Found"},
  {304, "Not Modified"},
  {400, "Bad Request"},
  {401, "Unauthorized"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {501, "Not Implemented"},
  {505, "HTTP Version Not Supported"}
};

struct codeToBody theCodeToBody[] = {
  {301, "<html>\r\n<head><title>301 Moved Permanently</title></head><body bgcolor=\"white\">\r\n<center><h1>301 Moved Permanently</h1></center>\r\n<center>httpd</center>\r\n</body></html>"},
  {400, "<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body bgcolor=\"white\">\r\n<center><h1>400 Bad Request</h1></center>\r\n\
          <hr><center>httpd</center>\r\n</body>\r\n</html>"},
  {404, "<html><title>NOT FOUND</title>\r\n<body><p>The server could not fulfil\r\nyour request because the resource specified\r\n\
          is not available or nonexisted.</body></html>"},
  {403, "<html><head><title>403 Forbidden</title></head>\r\n<body bgcolor=\"white\" youdao=\"bind\">\r\n<center><h1>403 Forbidden</h1></center>\r\n\
  <hr><center>nginx/1.0.15</center>\r\n</body></html>"}
};

Response newResponse();
void freeResponse(Response response);
void getPathFromUrl(char *url, char *path, int size);
char* getBodyFromCode(int code);
char* getDescriptionFromCode(int code);
void buildStartLine(int statusCode, ReponseStartLine startLine);
void buildFileHeaders(Response response, char* path);
void buildErrorHeaders(Response response, char* path);
void addRedirectLocation(Response response);

void printResponse(Response response)
{
  int i;
  printf("code: %d\n", response -> startLine -> statusCode);
  printf("description: %s\n", response -> startLine -> description);
  printf("version: %s\n", response -> startLine -> httpVersion);

  printf("headersSize: %d\n", response -> headersNumber);
  for (i = 0; i < response -> headersNumber; ++i)
  {
    printf("%s:%s\n", response -> headers[i] -> name, response -> headers[i] -> value);
  }
}

int handleResponse(int sockfd, Request request)
{
  char path[PATH_LEN];
  int fd;
  Response response;

  response = newResponse();

  getPathFromUrl(request -> startLine -> url, path, PATH_LEN);

  printf("path: %s\n", path);
  // check if file is existed or permission is allowed
  if (hcode == 200)
  {
    if (((fd = open(path, O_RDONLY))) < 0)
    {
      if (errno == EACCES)
        hcode = 403;
      else if (errno == ENOENT)
        hcode = 404;
    }
  }

  printf("hcode = %d\n", hcode);

  buildStartLine(hcode, response -> startLine);

  if (hcode != 200)
    buildErrorHeaders(response, path);
  else
    buildFileHeaders(response, path);

  if (hcode == 301)
    addRedirectLocation(response);

  printResponse(response);

  sendResponseHeaders(sockfd, response);

  if (hcode != 200)
    sendErrorBody(sockfd);
  else
    sendFileBody(sockfd, fd);
  
  freeRequest(request);
  freeResponse(response);
  hcode = SUCCESS;
  close(sockfd);
}

void getPathFromUrl(char *url, char *path, int size)
{
  int i, j;

  memset(path, 0, size);
  sprintf(path, "%s", FILE_DIRECTORY);

  j = strlen(path);
  for (i = 0; url[i] != '\0' && url[i] != '?' && i < size; ++i, ++j)
    path[j] = url[i];

  path[j] = '\0';
}

int sendFileBody(int sockfd, int filefd)
{
  char buff[BUFF_LEN];
  int n;

  while ((n = read(filefd, buff, BUFF_LEN)) > 0)
  {
    write(sockfd, buff, n);
  }
}

int sendErrorBody(int sockfd)
{
  char *body;

  body = getBodyFromCode(hcode);
  write(sockfd, body, strlen(body));
}

int sendResponseHeaders(int sockfd, Response response)
{
  char buff[BUFF_LEN];
  int i;

  memset(buff, 0, BUFF_LEN);
  sprintf(buff, "%s %d %s\r\n", response -> startLine -> httpVersion, response -> startLine -> statusCode, response -> startLine -> description);
  write(sockfd, buff, strlen(buff));

  for (i = 0; i < response -> headersNumber; ++i)
  {
    memset(buff, 0, BUFF_LEN);
    sprintf(buff, "%s:%s\r\n", response -> headers[i] -> name, response -> headers[i] -> value);
    write(sockfd, buff, strlen(buff));
  }

  memset(buff, 0, BUFF_LEN);
  sprintf(buff, "%s", "\r\n");
  write(sockfd, buff, strlen(buff));

  return 0;
}

char* getDescriptionFromCode(int code)
{
  int i, len;

  len = sizeof(theCodeToDescription) / sizeof(struct codeToDescription);

  for (i = 0; i < len; ++i)
  {
    if (theCodeToDescription[i].code == code)
      return theCodeToDescription[i].description;
  }
}

char* getBodyFromCode(int code)
{
  int i, len;

  len = sizeof(theCodeToBody) / sizeof(struct codeToBody);

  for (i = 0; i < len; ++i)
  {
    if (theCodeToBody[i].code == code)
      return theCodeToBody[i].body;
  }  
}

void buildStartLine(int statusCode, ReponseStartLine startLine)
{
  sprintf(startLine -> httpVersion, "%s", "HTTP/1.1");
  startLine -> statusCode = statusCode;
  sprintf(startLine -> description, "%s", getDescriptionFromCode(statusCode));
}

void addRedirectLocation(Response response)
{
  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));

  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Location");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", REDIRECT_URL); 
  ++response -> headersNumber;   
}

void buildFileHeaders(Response response, char* path)
{
  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));

  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "content-length");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%d", getContentLengthFromFile(path)); 
  ++response -> headersNumber; 

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "content-type");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", getContentTypeFromPath(path));    
  ++response -> headersNumber;

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", SERVER_NAME);
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", SERVER_VALUE);    
  ++response -> headersNumber;
}

void buildErrorHeaders(Response response, char* path)
{
  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "content-length");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%d",strlen(getBodyFromCode(hcode))); 
  ++response -> headersNumber; 

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "content-type");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", "text/html");    
  ++response -> headersNumber;

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", SERVER_NAME);
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", SERVER_VALUE);    
  ++response -> headersNumber;
}

Response newResponse()
{
  Response response;

  response = malloc(sizeof(struct response));
  response -> startLine = malloc(sizeof(struct responseStartLine));
  memset(response -> startLine, 0, sizeof(struct responseStartLine));

  response -> headersNumber = 0;
  response -> headers = malloc(sizeof(Header) * MAX_HEADERS_NUMBER);
  memset(response -> headers, 0, sizeof(Header) * MAX_HEADERS_NUMBER);

  response -> bodySize = 0;
  response -> body = NULL;

  return response;
}

void freeResponse(Response response)
{
  int i;

  if (response == NULL)
    return;

  if (response -> startLine != NULL)
    free(response -> startLine);

  for (i = 0; i < response -> headersNumber; ++i)
    if ((response -> headers)[i] != NULL)
      free (response -> headers[i]);

  if (response -> body != NULL)
    free(response -> body);

  free(response);
}