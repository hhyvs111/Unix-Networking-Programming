GCC = gcc
LIBXML2_ROOT = /usr/include/libxml2

LIBXML2 = -I $(LIBXML2_ROOT)

MY_LIB = -lxml2

test:TcpClient.c 
	$(GCC) TcpClient.c  -o Client $(MY_LIB) $(LIBXML2) 


