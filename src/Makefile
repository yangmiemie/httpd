objects = httpd.o content_type.o content_length.o

all: $(objects)
	cc -o httpd $(objects)

httpd.o: defs.h

content_type.o: content_type.h

content_length.o: content_length.h

clean:
	rm $(objects)