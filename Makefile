build:
	cd src && $(MAKE)

install:
	cd src && ./httpd

test:
	cd tests && $(MAKE) && ./test