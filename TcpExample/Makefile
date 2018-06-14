GCC = gcc 
CFLAGS =   -g
LIBXML2_ROOT = /usr/include/libxml2
LIBTHREAD = -lpthread
LIBXML2 = -I $(LIBXML2_ROOT)

MY_LIB = -lxml2

#objects = TcpServer.o TcpClient.o readXML.o 
all : Server Client ProduceXML 


Server : TcpServer.c readXML.c 
	$(GCC) $(CFLAGS) -o $@ $^ $(MY_LIB) $(LIBXML2)  $(LIBTHREAD)

Client : TcpClient.o 
	$(GCC) $(CFLAGS) -o $@ $^ $(MY_LIB) $(LIBXML2) 

ProduceXML : produceXML.o
	$(GCC) $(CFLAGS) -o $@  $^ $(MY_LIB) $(LIBXML2) 

produceXML.o : produceXML.c
	$(GCC)  $(CFLAGS) -c $^ $(MY_LIB) $(LIBXML2)
#readXML.o : readXML.c 
	#$(GCC)  $(CFLAGS) -c $^ $(MY_LIB) $(LIBXML2)

#TcpServer.o : TcpServer.c readXML.c  
   # $(GCC)  $(CFLAGS) -c $^  $(MY_LIB) $(LIBXML2)  

TcpClient.o : TcpClient.c 
	$(GCC)  $(CFLAGS) -c TcpClient.c $(MY_LIB) $(LIBXML2)


