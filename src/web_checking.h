#define Request struct request*

int checkMethodOfRequest(Request request);
int checkHostOfRequest(Request request);

#undef Request
