objects = httpd.o content_type.o content_length.o utils.o request.o response.o

all: $(objects)
	cc -o httpd $(objects)

httpd.o: defs.h httpd.h

content_type.o: content_type.h

content_length.o: content_length.h

utils.o: utils.h

request.o: error.h request.h utils.h

response.o: error.h response.h

clean:
	rm $(objects) || rm a.out