#define FILETYPELEN 8
#define CONTENTTYPELEN 16

struct contentType
{
  char* fileType;
  char* contentType;
};

char* getContentTypeFromFileType(const char*);
int getFileTypeFromPath(const char*, char*, int);
char* getContentTypeFromPath(const char*);

