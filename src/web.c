#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "web.h"

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

    if (checkLastModifiedOfFile(request) < 0)
    {
      printf("return 304\n");
      handleResponse(sockfd, request);
      return;
    }
    // at last, send response with 200 and serve file
    hcode = 200;
    handleResponse(sockfd, request);
  }
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
