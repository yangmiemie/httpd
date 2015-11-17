int startup();
void web(int sockfd);

#define HTTP_PORT 8080
#define LISTENQ 16
#define MAXLINE 256
#define MAXRESPONSE 1024

#define WORKER_PROCESSES 4

int port;
int nchildren;
