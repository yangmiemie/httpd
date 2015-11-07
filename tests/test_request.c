#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "../src/request.h"
#include "../src/error.h"

void testParseStartLineOfRequest()
{
  char *buf;
  StartLine startLine;
  int n;

  startLine = malloc(sizeof(struct startLine));

  buf = "Get /finance/test HTTP/1.1";
  n = parseStartLineOfRequest(buf, strlen(buf), startLine);
  assert(n == SUCCESS);

  assert(strcmp("Get", startLine -> method) == 0);
  assert(strcmp("/finance/test", startLine -> url) == 0);
  assert(strcmp("HTTP/1.1", startLine -> httpVersion) == 0);

  buf = "Get /finance/test";
  n = parseStartLineOfRequest(buf, strlen(buf), startLine);
  assert(n == ERROR);
  assert(herrno == BAD_REQUEST);

  buf = "Get";
  n = parseStartLineOfRequest(buf, strlen(buf), startLine);
  assert(n == ERROR);
  assert(herrno == BAD_REQUEST);
  
  buf = "Get";
  n = parseStartLineOfRequest(buf, strlen(buf), startLine);
  assert(n == ERROR);
  assert(herrno == BAD_REQUEST);
}

void testParseHeaderOfRequest()
{
  char *buf;
  Header header;
  int n;

  header = malloc(sizeof(struct header));
  buf = "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36";

  n = parseHeaderOfRequest(buf, strlen(buf), header);
  assert(n == SUCCESS);
  assert(strcmp(header -> name, "User-Agent") == 0);
  assert(strcmp(header -> value, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36") == 0);

  buf = "User-Agent Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36";
  n = parseHeaderOfRequest(buf, strlen(buf), header);
  assert(n == ERROR);
  assert(herrno == BAD_REQUEST);
}

int main(int argc, char const *argv[])
{
  testParseStartLineOfRequest();
  testParseHeaderOfRequest();
  return 0;
}