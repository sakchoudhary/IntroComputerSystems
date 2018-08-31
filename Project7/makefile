# makefile

all: dataserver client

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -std=c++11 -c -g NetworkRequestChannel.cpp

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -std=c++11 -c -g reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -std=c++11 -c -g BoundedBuffer.cpp

dataserver: dataserver.cpp reqchannel.o NetworkRequestChannel.o
	g++ -std=c++11 -g -o dataserver dataserver.cpp reqchannel.o NetworkRequestChannel.o -lpthread

client: client.cpp reqchannel.o BoundedBuffer.o 
	g++ -std=c++11 -g -o client client.cpp reqchannel.o BoundedBuffer.o NetworkRequestChannel.o -lpthread

clean:
	rm -rf *.o fifo* dataserver client
