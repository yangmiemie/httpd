#include <ctype.h>

char* stringToLower(char* string)
{
  int i;

  for (i = 0; string[i] != '\0'; ++i)
  {
    string[i] = tolower(string[i]);
  }

  return;
}