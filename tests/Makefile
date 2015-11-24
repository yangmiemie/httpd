objects = test.o test_request.o test_content_type.o request.o content_type.o range.o\
					test_content_length.o content_length.o test_range.o utils.o

all : $(objects)
	cc -o test $(objects) && rm $(objects)

test.o: test.h

test_request.o: ../src/request.h

test_content_type.o: ../src/content_type.h

test_content_length.o: ../src/content_length.h

test_range.o: ../src/range.h

request.o: ../src/request.h ../src/utils.h
	cc -c ../src/utils.c ../src/request.c

content_type.o: ../src/content_type.h
	cc -c ../src/content_type.c

content_length.o: ../src/content_length.h
	cc -c ../src/content_length.c

utils.o: ../src/utils.h

range.o: ../src/range.h
	cc -c ../src/range.c

clean:
	rm $(objects)