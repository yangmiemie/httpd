int handleResponse(int sockfd, struct request* request);
void cat(int sockfd, int fd);
void buildHeader(int sockfd, char* path);
void printResponse(struct response* response);