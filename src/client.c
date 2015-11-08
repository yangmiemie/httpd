#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MAXLINE 64

int main(int argc, char const *argv[])
{
  struct sockaddr_in servaddr;
  int sockfd, infd;
  int i, n;
  char request[MAXLINE], reply[MAXLINE];

  if (argc != 3)
  {
    fprintf(stderr, "usage: cliselect <ip> <port>\n");
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket error");
    exit(1);
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr) < 0)
  {
    perror("inet_pton error");
    exit(1);
  }

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("connect error");
    exit(1);
  }

  n = sprintf(request, "GET /12.html HTTP/1.1\r\n");

  if (write(sockfd, request, strlen(request)) < 0)
  {
    perror("write error");
    exit(1);
  }

  n = sprintf(request, "host: 127.0.0.1\r\n");
  if (write(sockfd, request, strlen(request)) < 0)
  {
    perror("write error");
    exit(1);
  }

  n = sprintf(request, "Accept: text/html\r\n");
  if (write(sockfd, request, strlen(request)) < 0)
  {
    perror("write error");
    exit(1);
  }

  // shutdown(sockfd, SHUT_WR);

  n = sprintf(request, "\r\n");
  if (write(sockfd, request, strlen(request)) < 0)
  {
    perror("write error");
    exit(1);
  }

  for ( ; ; )
  {
    if ((n = read(sockfd, reply, sizeof(reply))) < 0)
    {
      perror("read error");
      exit(1);
    }
    else if (n == 0)
    {
      fprintf(stderr, "server teminated\n");
      break;
    }

    write(1, reply, n);
  }
}