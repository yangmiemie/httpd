# Httpd, A Simple Web Server

## HTTP Code

#### 201
* create a new resource in server

#### 206
* partial content, when response to range header

#### 301
* redirect to 127.0.0.1 when host is localhost

#### 304
* return 304 when file is not modified

#### 400
* host is not existed or invalid
* format of header is invalid
* method is invalid

#### 403
* the request access to the file is not allowed, or search permission is denied for one of directory in the path prefix of pathname

#### 404
* file is not existed in server

#### 411
* post without content-length

#### 416 (Requested Range Not Satisfiable)
* format of bytes in range is invalid, such as Range: bytes=a-10

#### 500
* server open dumpfile error


## HTTP Method

Range: bytes=m-n


Need to be done:
Content-Range
Method not allowed
Not implemented
gzip



