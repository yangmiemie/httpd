struct request;
#define Request struct request*
struct requestStartLine;
#define RequestStartLine struct requestStartLine*
struct header;
#define Header struct header*

int acceptRequest(int sockfd, Request request);
int readByteFromSocket(int sockfd);
int readLineFromSocket(int sockfd, char* line, int size);
int parseStartLineOfRequest(char* buf, int size, RequestStartLine requestStartLine);
int parseHeaderOfRequest(char *buf, int size, Header header);
char* getHostOfRequest(Request request);
Request newRequest();
void freeRequest(Request request);
void printRequest(Request request);

#define ISSPACE(x) (((x) == ' ') ? 1 : 0)
#define ISCOLON(x) (((x) == ':') ? 1 : 0)