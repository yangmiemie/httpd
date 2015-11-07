#define FILETYPELEN 8
#define CONTENTTYPELEN 16

struct contentType
{
  char* fileType;
  char* contentType;
};

char* getContentTypeFromFileType(char*);
int getFileTypeFromPath(char*, char*, int);
char* getContentTypeFromPath(char*);

