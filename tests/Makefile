objects = test.o test_request.o test_content_type.o request.o content_type.o \
					test_content_length.o content_length.o

all : $(objects)
	cc -o test $(objects) && rm $(objects)

test.o: test.h

test_request.o: ../src/request.h

test_content_type.o: ../src/content_type.h

test_content_length.o: ../src/content_length.h

request.o: ../src/request.h
	cc -c ../src/request.c

content_type.o: ../src/content_type.h
	cc -c ../src/content_type.c

content_length.o: ../src/content_length.h
	cc -c ../src/content_length.c

clean:
	rm $(objects)