#include "http.h"
#include "response.h"
#include "request.h"
#include "error.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
  {206, "Partial Content"},
  {301, "Moved Permanently"},
  {302, "Found"},
  {304, "Not Modified"},
  {400, "Bad Request"},
  {401, "Unauthorized"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {411, "Length Required"},
  {416, "Requested Range Not Satisfiable"},
  {500, "Internal Server Error"},
  {501, "Not Implemented"},
  {505, "HTTP Version Not Supported"}
};

struct codeToBody theCodeToBody[] = {
  {201, "<html>\r\n<head><title>201 Created</title></head><body bgcolor=\"white\">\r\n<center><h1>200 Created</h1></center>\r\n<center>httpd</center>\r\n</body></html>"},
  {301, "<html>\r\n<head><title>301 Moved Permanently</title></head><body bgcolor=\"white\">\r\n<center><h1>301 Moved Permanently</h1></center>\r\n<center>httpd</center>\r\n</body></html>"},
  {400, "<html>\r\n<head><title>400 Bad Request</title></head>\r\n<body bgcolor=\"white\">\r\n<center><h1>400 Bad Request</h1></center>\r\n\
          <hr><center>httpd</center>\r\n</body>\r\n</html>"},
  {403, "<html><head><title>403 Forbidden</title></head>\r\n<body bgcolor=\"white\" youdao=\"bind\">\r\n<center><h1>403 Forbidden</h1></center>\r\n\
  <hr><center>nginx/1.0.15</center>\r\n</body></html>"},
  {404, "<html><title>NOT FOUND</title>\r\n<body><p>The server could not fulfil\r\nyour request because the resource specified\r\n\
          is not available or nonexisted.</body></html>"},
  {405, "<html>\r\n<head><title>405 Method Not Allowed</title></head>\r\n<body bgcolor=\"white\">\r\n<center><h1>405 Method Not Allowed</h1></center>\r\n\
          <hr><center>httpd</center>\r\n</body>\r\n</html>"},
  {411, "<html>\r\n<head><title>Length Required</title></head>\r\n<body><h1>Length Required</h1>\r\n\
        <center>HTTPD::HTTPStatus::LengthRequired</center></body>\r\n</html>"},
  {416, "<html>\r\n<head><title>416 Requested Range Not Satisfiable</title></head>\r\n<body bgcolor=\"white\">\r\n<center><h1>416 Requested Range Not Satisfiable</h1></center>\r\n\
          <hr><center>httpd</center>\r\n</body>\r\n</html>"},
  {500, "<html>\r\n<head><title>500 Internal Server Error</title></head>\r\n<body><h1>Internal Server Error</h1>\r\n\
        <center>HTTPD::HTTPStatus::InternalServerError</center></body>\r\n</html>"},
  {501, "<html>\r\n<head><title>501 Not Implemented</title></head>\r\n<body bgcolor=\"white\">\r\n<center><h1>501 Not Implemented</h1></center>\r\n\
          <hr><center>httpd</center>\r\n</body>\r\n</html>"}
};

Response newResponse();
void freeResponse(Response response);
void getPathFromUrl(char *url, char *path, int size);
char* getBodyFromCode(int code);
char* getDescriptionFromCode(int code);
void buildStartLine(int statusCode, ReponseStartLine startLine);
void buildFileHeaders(Response response, char* path, int length);
void buildErrorHeaders(Response response, char* path);
void addRedirectLocation(Response response);
void addContentRange(Response response, char* path, int offset, int len);

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
  int fd, offset, len;
  char *ptr;
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

  offset = 0;
  len = BUFF_LEN;
  if ((ptr = getHeaderOfRequest(request, "range")) != NULL)
  {
    printf("getRangeOfRequest ptr: %s\n", ptr);
    if (getRangeOfRequest(ptr, path, &offset, &len) < 0)
      hcode = 416;
    else
      hcode = 206;
  }

  printf("after getRangeOfRequest, hcode = %d\n", hcode);
  buildStartLine(hcode, response -> startLine);

  if (hcode == 200 || hcode == 206)
    buildFileHeaders(response, path, len);
  else
    buildErrorHeaders(response, path);

  printf("after buildFileHeaders\n");

  if (hcode == 301)
    addRedirectLocation(response);

  if (hcode = 206)
    addContentRange(response, path, offset, len);

  printResponse(response);

  sendResponseHeaders(sockfd, response);

  if (hcode == 200 || hcode == 206)
    sendFileBody(sockfd, fd, offset, len);
  else if (hcode == 304)
  {}
  else
    sendErrorBody(sockfd);

  freeRequest(request);
  freeResponse(response);
  hcode = SUCCESS;
}

int sendFileBody(int sockfd, int filefd, int offset, int len)
{
  char buff[BUFF_LEN];
  int n;

  if ((hcode == 206) && (offset > 0))
  {
    if ((n = lseek(filefd, offset, SEEK_SET)) < 0)
    {
      perror("lseek error");
      exit(1);
    }
  }

  while ((n = read(filefd, buff, len)) > 0)
  {
    write(sockfd, buff, n);

    if (n >= len)
      break;
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

void addContentRange(Response response, char* path, int offset, int len)
{
  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));

  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Content-Range");
  sprintf((response -> headers[response -> headersNumber]) -> value, "bytes %d-%d/%d", offset, offset + len - 1, getContentLengthFromFile(path)); 
  ++response -> headersNumber;   
}

// the length paramter means the length of content, when 206, the length is not the size of file
void buildFileHeaders(Response response, char* path, int length)
{
  char modifiedTimeOfFile[TIME_LEN];
  int contentLength;

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));

  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Content-length");
  contentLength = hcode == 206 ? length : getContentLengthFromFile(path);
  sprintf((response -> headers[response -> headersNumber]) -> value, "%d", contentLength); 
  ++response -> headersNumber; 

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Content-type");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", getContentTypeFromPath(path));    
  ++response -> headersNumber;

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", SERVER_NAME);
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", SERVER_VALUE);    
  ++response -> headersNumber;

  printf("path: %s\n", path);
  getLastModifiedTime(path, modifiedTimeOfFile, TIME_LEN);

  printf("Last-Modified: %s", modifiedTimeOfFile);

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Last-Modified");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", modifiedTimeOfFile);    
  ++response -> headersNumber;

  response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
  memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
  
  sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Cache-Control");
  sprintf((response -> headers[response -> headersNumber]) -> value, "%s", "max-age=315360000");    
  ++response -> headersNumber;  
}

void buildErrorHeaders(Response response, char* path)
{
  if (hcode != 304)
  {
    response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
    memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
    
    sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Content-length");
    sprintf((response -> headers[response -> headersNumber]) -> value, "%d",strlen(getBodyFromCode(hcode))); 
    ++response -> headersNumber; 

    response -> headers[response -> headersNumber] = malloc(sizeof(struct header));
    memset(response -> headers[response -> headersNumber], 0, sizeof(struct header));
    
    sprintf((response -> headers[response -> headersNumber]) -> name, "%s", "Content-type");
    sprintf((response -> headers[response -> headersNumber]) -> value, "%s", "text/html");    
    ++response -> headersNumber;    
  }

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