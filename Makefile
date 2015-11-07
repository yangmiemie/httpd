build:
	cd src && $(MAKE)

install:
	cd src && ./httpd

test:
	cd tests && $(MAKE) && ./test

clean:
	cd tests && rm *.o && cd ../src && rm *.o