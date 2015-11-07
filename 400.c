void not_found(int sockfd, char *path)
{
  char buff[MAXLINE];

  sprintf(buff, "HTTP/1.0 404 NOT FOUND\r\n");
  write(sockfd, buff, strlen(buff));
  write(sockfd, SERVER_STRING, strlen(SERVER_STRING));
  sprintf(buff, "content-type: text/html\r\n\r\n");
  write(sockfd, buff, strlen(buff));
  sprintf(buff, "<html><title>NOT FOUND</title>\r\n");
  write(sockfd, buff, strlen(buff));
  sprintf(buff, "<body><p>The server could not fulfil\r\n");
  write(sockfd, buff, strlen(buff));
  sprintf(buff, "your request because the resource specified\r\n");
  write(sockfd, buff, strlen(buff));
  sprintf(buff, "is not available or nonexisted.");
  write(sockfd, buff, strlen(buff));
  sprintf(buff, "</body></html>");
  write(sockfd, buff, strlen(buff));
}
