#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "defs.h"
#include "web.h"

int handlePostofRequest(int sockfd, Request request);

void web(int sockfd)
{
  Request request;
  int n;

  for ( ; ; )
  {
    request = newRequest();

    if ((n = acceptRequest(sockfd, request)) == ERROR)
    {
      handleResponse(sockfd, request);
      return;
    }
    else if (n == FIN_CODE)
    {
      // if receive FIN, then return 
      printf("receive FIN_CODE\n");
      return;
    }

    printRequest(request);
    
    if (checkMethodOfRequest(request) < 0)
    {
      printf("method error\n");
      handleResponse(sockfd, request);
      return;
    }

    if (checkHostOfRequest(request) < 0)
    {
      printf("host error\n");
      handleResponse(sockfd, request);
      return;
    }

    if (strcmp(stringToLower(request -> startLine -> method), "get") == 0)
    {

      if (checkLastModifiedOfFile(request) < 0)
      {
        printf("return 304\n");
        handleResponse(sockfd, request);
        return;
      }
    }
    else if (strcmp(stringToLower(request -> startLine -> method), "post") == 0)
    {
      if (getHeaderOfRequest(request, "content-length") == NULL)
      {
        hcode = 411;
        handleResponse(sockfd, request);
        return;
      }

      handlePostofRequest(sockfd, request);
      handleResponse(sockfd, request);
      return;
    }

    // at last, send response with 200 and serve file
    hcode = 200;
    handleResponse(sockfd, request);
  }
}

int handlePostofRequest(int sockfd, Request request)
{
  int len, n;
  int fd;

  len = atoi(getHeaderOfRequest(request, "content-length"));

  request -> body = malloc(len);
  request -> bodySize = len;

  if ((n = read(sockfd, request -> body, len)) < 0)
  {
    hcode = 400;
    return ERROR;
  }

  if ((fd = open(DB_FILE, O_RDWR)) < 0)
  {
    hcode = 500;
    return ERROR;
  }

  lseek(fd, 0, SEEK_END);

  if (write(fd, request -> body, request -> bodySize) < 0)
  {
    hcode = 500;
    return ERROR;
  }

  write(fd, "\r\n", 2);

  hcode = 201;
  return SUCCESS;
}

void child_main(int listenfd)
{
  struct sockaddr_in cliaddr;
  int clilen;
  int connfd;
  
  clilen = sizeof(cliaddr);

  for ( ; ; )
  {
    if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
    {
      perror("accept error");
      exit(1);
    }

    web(connfd);
    close(connfd);
  }
}

int child_make(int i, int listenfd)
{
  pid_t pid;

  if ((pid = fork()) > 0)
    return pid;

  printf("child %d created\n", i);
  child_main(listenfd);
}
