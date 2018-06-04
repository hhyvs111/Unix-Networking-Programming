GCC = gcc 
LIBXML2_ROOT = /usr/include/libxml2

LIBXML2 = -I $(LIBXML2_ROOT)

MY_LIB = -lxml2

all : Server Client

Server : TcpServer.o readXML.o read-write.o
	$(GCC) -g -o Server $(MY_LIB) $(LIBXML2) TcpServer.o readXML.o read-write.o 

Client : TcpClient.o 
	$(GCC) -g -o Client $(MY_LIB) $(LIBXML2)


TcpServer.o : TcpServer.c 
	$(GCC) -c TcpServer.c $(MY_LIB) $(LIBXML2)  

readXML.o : readXML.c
	$(GCC) -c readXML.c $(MY_LIB) $(LIBXML2)  

read-write.o : read-write.c
	$(GCC) -c read-write.c $(MY_LIB) $(LIBXML2) 
#clean : 
	#rm Server
	#rm Client 
   # rm *.o

#Clinet:TcpClient.c 
	#$(GCC) TcpClient.c  -o Client $(MY_LIB) $(LIBXML2) 

#Server:TcpServer.c readXML.c
	#$(GCC) TcpServer.c readXML.c -o Server $(MY_LIB) $(LIBXML2)
