GCC = gcc 
LIBXML2_ROOT = /usr/include/libxml2
LIBTHREAD = -lpthread
LIBXML2 = -I $(LIBXML2_ROOT)

MY_LIB = -lxml2

#objects = TcpServer.o TcpClient.o readXML.o 
all : Server Client ProduceXML 


Server : TcpServer.o readXML.o 
	$(GCC) -g -o Server TcpServer.o readXML.o  $(MY_LIB) $(LIBXML2)  $(LIBTHREAD)

Client : TcpClient.o 
	$(GCC) -g -o Client TcpClient.o $(MY_LIB) $(LIBXML2) 

ProduceXML : produceXML.o
	$(GCC) -g -o produceXML  produceXML.o $(MY_LIB) $(LIBXML2) 

produceXML.o : produceXML.c
	$(GCC) -c produceXML.c $(MY_LIB) $(LIBXML2)
readXML.o : readXML.c 
	$(GCC) -c readXML.c $(MY_LIB) $(LIBXML2)

TcpServer.o : TcpServer.c readXML.c  
	$(GCC) -c TcpServer.c readXML.c $(MY_LIB) $(LIBXML2)  

TcpClient.o : TcpClient.c 
	$(GCC) -c TcpClient.c $(MY_LIB) $(LIBXML2)


