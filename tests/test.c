#include "test.h"

int main(int argc, char const *argv[])
{
  testParseStartLineOfRequest();
  testParseHeaderOfRequest();

  testGetFileTypeFromPath();
  testGetContentTypeFromPath();
  testGetContentTypeFromFileType();

  testGetContentLengthFromFile();

  testRangeOfRequest();
  
  return 0;
}