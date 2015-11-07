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

#define HTTP_PORT 8080
#define LISTENQ 16
#define MAXLINE 256
#define MAXRESPONSE 1024
#define SERVER_STRING "server: httpd\r\n"

#define ISSPACE(x) (((x) == ' ') ? 1 : 0)
int startup();
void accept_request(int sockfd);
int get_line(int sockfd, char* line, int size);
void cat(int sockfd, int fd);
void header(int sockfd, char *path);
void serve_file(int sockfd, char* path);
void not_implemented(int sockfd);

int port;

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

void accept_request(int sockfd)
{
  char line[MAXLINE], path[MAXLINE];
  int len;
  char *ptrFront, *ptrBack;

  printf("in accept_request, sockfd = %d\n", sockfd);
  len = get_line(sockfd, line, sizeof(line));

  printf("First Line: %s\n", line);

  ptrFront = line;
  ptrBack = line;
  while (!ISSPACE(*ptrFront) && (*ptrFront != '\0'))
  {
    *ptrFront = tolower(*ptrFront);
    ++ptrFront;    
  }

  *ptrFront++ = '\0';

  if ((strcmp(ptrBack, "get") != 0))
  {
    not_implemented(sockfd);
    return;
  }

  printf("method: %s\n", ptrBack);

  while (ISSPACE(*ptrFront) && ptrFront != '\0')
    ++ptrFront;

  memset(path, 0, sizeof(path));
  sprintf(path, "/var/httpd");
  ptrBack = &path[strlen(path)];
  while (!ISSPACE(*ptrFront) && (*ptrFront != '\0'))
  {
    *ptrBack = *ptrFront;
    ++ptrFront;
    ++ptrBack;
  }

  *ptrBack = '\0';

  ptrBack = path;
  while ((*ptrBack != '?') && (*ptrBack != '\0'))
    ++ ptrBack;

  *ptrBack = '\0';

  if (path[strlen(path) - 1] == '/')
    strcat(path, "index.html");

  printf("path: %s\n", path);

  serve_file(sockfd, path);

  close(sockfd);
}

int get_line(int sockfd, char* line, int size)
{
  int c, i, n;

  i = 0;
  c = 0;

  while (i != size - 1)
  {
    if ((n = recv(sockfd, &c, 1, 0)) < 0)
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
      
      break;
    }
    else
    {
      line[i++] = c;
    }
  }

  line[i] = '\0';
  return i;
}

void not_implemented(int sockfd)
{

}

void cat(int sockfd, int fd)
{
  char buff[MAXRESPONSE];
  int n;

  while ((n = read(fd, buff, MAXRESPONSE)) > 0)
  {
    write(sockfd, buff, n);
  }
}

void header(int sockfd, char* path)
{
  char buff[MAXLINE];
  int n;

  n = sprintf(buff, "HTTP/1.1 200 OK\r\n");
  write(sockfd, buff, n);
  n = sprintf(buff, "content-type: %s\r\n", getContentTypeFromPath(path));
  write(sockfd, buff, n);
  n = write(sockfd, SERVER_STRING, strlen(SERVER_STRING));
  n = sprintf(buff, "content-length: %d\r\n\r\n", getContentLengthFromFile(path));
  write(sockfd, buff, n);
}

void setunblocking(int sockfd)
{
  int flags;

  flags = fcntl(sockfd, F_GETFL, 0);
  fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

}

void serve_file(int sockfd, char* path)
{
  char buff[MAXLINE];
  int fd, n;

  setunblocking(sockfd);

  // read all left bytes using unblocking io
  while ((n = read(sockfd, buff, MAXLINE)) > 0)
  {
    printf("n = %d\n", n);
    buff[n] = '\0';
    printf("%s\n", buff);    
  }

  printf("end of reading\n");
  if ((fd = open(path, O_RDONLY)) < 0)
  {
    not_found(sockfd, path);
    return;
  }

  header(sockfd, path);
  cat(sockfd, fd);  
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
    accept_request(connfd);
  }

  return 0;
}