objects = httpd.o content_type.o content_length.o utils.o request.o response.o web_checking.o web.o \
					last_modified.o 

all: $(objects)
	cc -o httpd $(objects)

httpd.o: defs.h httpd.h web.h

web.o: defs.h web.h

content_type.o: content_type.h

content_length.o: content_length.h

last_modified.o: last_modified.h

utils.o: utils.h

request.o: error.h request.h utils.h

response.o: error.h response.h

web_checking.o:  web_checking.h utils.h

clean:
	rm $(objects)
	rm httpd
	rm a.out
