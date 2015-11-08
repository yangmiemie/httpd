#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "defs.h"
#include "httpd.h"
#include "utils.h"

int startup()
{
  int listenfd;
  struct sockaddr_in servaddr;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket error");
    exit(1);
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind error");
    exit(1);
  }

  if (listen(listenfd, LISTENQ) < 0)
  {
    perror("listen error");
    exit(1);
  }

  return listenfd;
}

char* getHostOfRequest(Request request)
{
  int i;

  for (i = 0; i < request -> headersNumber; ++i)
  {
    if (strcmp(stringToLower(((request -> headers)[i]) -> name), "host") == 0)
      return (request -> headers[i]) -> value;
  }
  
  return NULL;
}

void web(int sockfd)
{
  Request request;
  int n;
  char *host;

  request = newRequest();

  if ((n = acceptRequest(sockfd, request)) < 0)
  {
    handleResponse(sockfd, request);
    return;
  }

  if ((host = getHostOfRequest(request)) == NULL)
  {
    hcode = 400;
    handleResponse(sockfd, request);
    return;
  }

  if ((strcmp(host, "127.0.0.1") != 0) && (strcmp(host, "localhost") != 0))
  {
    hcode = 400;
    handleResponse(sockfd, request);
    return;
  }

  if (strcmp(host, "localhost") == 0)
  {
    hcode = 301;
    handleResponse(sockfd, request);
    return;
  }

  // at last, send response with 200 and serve file
  hcode = 200;
  handleResponse(sockfd, request);

  close(sockfd);
}


int main(int argc, char const *argv[])
{
  int listenfd, connfd;
  struct sockaddr_in cliaddr;
  int clilen;

  port = HTTP_PORT;

  if (argc == 2)
  {
    port = atoi(argv[1]);
  }

  listenfd = startup();
  clilen = sizeof(cliaddr);


  for ( ; ; )
  {
    if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
    {
      perror("accept error");
      exit(1);
    }

    printf("accept connfd = %d\n", connfd);
    web(connfd);
  }

  return 0;
}