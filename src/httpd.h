int startup();
void web(int sockfd);

#define HTTP_PORT 8080
#define LISTENQ 16
#define MAXLINE 256
#define MAXRESPONSE 1024

int port;
