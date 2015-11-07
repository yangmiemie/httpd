#define METHOD_LEN 8
#define URL_LEN 64
#define HTTP_VERSION_LEN 8

#define HEADER_NAME_LEN 16
#define HEADER_VALUE_LEN 512

#define START_LINE_LEN 128
#define HEADER_LEN 512
#define BODY_LEN 1024

typedef struct request* Request;
typedef struct header* Header;
typedef struct startLine* StartLine;

struct header
{
  char name[HEADER_NAME_LEN];
  char value[HEADER_VALUE_LEN];
};

struct startLine
{
  char method[METHOD_LEN];
  char url[URL_LEN];
  char httpVersion[HTTP_VERSION_LEN];
};

struct request 
{
  StartLine startLine;
  Header *headers;
  int headersNumber;
  char body[BODY_LEN];
};

int acceptRequest(int sockfd, Request request);
int readByteFromSocket(int sockfd);
int readLineFromSocket(int sockfd, char* line, int size);
int parseStartLineOfRequest(char* buf, int size, StartLine requestStartLine);
int parseHeaderOfRequest(char *buf, int size, Header header);

#define ISSPACE(x) (((x) == ' ') ? 1 : 0)
#define ISCOLON(x) (((x) == ':') ? 1 : 0)