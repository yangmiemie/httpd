typedef struct response* Response;
typedef struct request* Request;
typedef struct header* Header;
typedef struct responseStartLine* ReponseStartLine;
typedef struct requestStartLine* RequestStartLine;

#define METHOD_LEN 8
#define URL_LEN 64
#define PATH_LEN 64
#define HTTP_VERSION_LEN 8
#define DESCRIPTION_LEN 16

#define HEADER_NAME_LEN 16
#define HEADER_VALUE_LEN 512

#define START_LINE_LEN 128
#define HEADER_LEN 512
#define REQUEST_BODY_LEN 1024

#define BUFF_LEN 512

#define FILE_DIRECTORY "/var/httpd"

#define SERVER_NAME "server"
#define SERVER_VALUE "httpd"

struct header
{
  char name[HEADER_NAME_LEN];
  char value[HEADER_VALUE_LEN];
};

struct requestStartLine
{
  char method[METHOD_LEN];
  char url[URL_LEN];
  char httpVersion[HTTP_VERSION_LEN];
};

struct responseStartLine
{
  char httpVersion[HTTP_VERSION_LEN];
  int statusCode;
  char description[DESCRIPTION_LEN];
};

struct request 
{
  RequestStartLine startLine;
  Header *headers;
  int headersNumber;
  char body[REQUEST_BODY_LEN];
};

struct response
{
  ReponseStartLine startLine;
  Header* headers;
  int headersNumber;
  char *body;
  int bodySize;
};

int hcode;