objects = test_request.o request.o

test : $(objects)
	cc -o test $(objects) && rm $(objects)

test_request.o: ../src/request.h

request.o: ../src/request.h
	cc -c ../src/request.c

clean:
	rm $(objects)