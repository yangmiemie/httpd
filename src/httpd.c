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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "httpd.h"
#include "web.h"

void sig_int(int signo);
static pid_t *pids;

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

int main(int argc, char const *argv[])
{
  int listenfd;
  int i;

  port = HTTP_PORT;

  if (argc == 2)
  {
    port = atoi(argv[1]);
  }

  listenfd = startup();

  nchildren = WORKER_PROCESSES;
  pids = calloc(nchildren, sizeof(pid_t));
  
  printf("nchildren = %d\n", nchildren);

  for (i = 0; i < nchildren; ++i)
  {
    pids[i] = child_make(i, listenfd);
  }

  signal(SIGINT, sig_int);

  for ( ; ; )
    pause();

  return 0;
}


void sig_int(int signo)
{
  int i;

  for (i = 0; i < nchildren; ++i)
  {
    kill(pids[i], SIGTERM);
  }

  while(wait(NULL) > 0);

  exit(1);
}
