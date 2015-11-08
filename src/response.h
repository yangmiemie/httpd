int handleResponse(int sockfd, struct request* request);
void cat(int sockfd, int fd);
void buildHeader(int sockfd, char* path);
void serve_file(int sockfd, char* path);
void setunblocking(int sockfd);