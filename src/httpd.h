int startup();
void accept_request(int sockfd);
int get_line(int sockfd, char* line, int size);
void cat(int sockfd, int fd);
void header(int sockfd, char *path);
void serve_file(int sockfd, char* path);
void not_implemented(int sockfd);

int httpErrorno;