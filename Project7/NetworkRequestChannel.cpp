#include "NetworkRequestChannel.h"

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

using namespace std;

#define MAXDATASIZE 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/* Creates a CLIENT-SIDE local copy of the channel. 
The channel is connected to the given port number at the given server host. 
THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short int _port_no){
    //cout << "connecting to server...\n";
    struct addrinfo hints, *serv,  *p;
    memset(&hints, 0, sizeof hints) ;
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM; 
    int status,numbytes;
    char s[INET6_ADDRSTRLEN];
    char buf[MAXDATASIZE];


    if(status = getaddrinfo(_server_host_name.c_str(),to_string(_port_no).c_str(), &hints, &serv) !=0){
       perror("res: getaddrinfo error"); 
       exit(-1); 
    }


    if((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1){
        perror("cs : socket eror "); 
        // continue; 
        exit(-1); 
    } 
    
    if(connect(sockfd,serv->ai_addr, serv->ai_addrlen) == -1){
        perror("cs: connect error"); 
        //continue;
        exit(-1); 
    }


    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(2);
    }


    // printf ("Successfully connected to the server %s\n", server_name);
	// printf ("Now Attempting to send a message to the server\n", server_name);
    
    //IS THIS NECESSARY??
    freeaddrinfo(serv); 
    // char buf [1024];
	// sprintf (buf, "hello");
	// send (sockfd, buf, strlen (buf)+1, 0);
  

    
}
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void*(*connection_handler)(void*), int backlog){
    struct addrinfo hints, *serv; 
    struct sockaddr_storage other_adr; 
    socklen_t sin_size; 
    char s[INET6_ADDRSTRLEN];
    int rv ;
    int socket_fd, new_fd; 
   
   memset(&hints, 0, sizeof hints ) ;
   hints.ai_family = AF_UNSPEC; 
   hints.ai_socktype = SOCK_STREAM; 
   hints.ai_flags = AI_PASSIVE; 

   if((rv = getaddrinfo(NULL,to_string(_port_no).c_str(), &hints, &serv)) != 0){
       perror("getaddrinfo"); 
       exit(-1); 
   }
   if((socket_fd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1){
       perror("server : socket"); 
       exit(-1); 
   }
   if(bind(socket_fd,serv->ai_addr, serv->ai_addrlen) == -1){
       close(socket_fd);
       perror("server bind"); 
       exit(-1); 
   }
   freeaddrinfo(serv); 
   //
   if(listen(socket_fd,backlog) == -1){
       perror("listen"); 
       exit(1); 
   }
   printf("server waiting for connections\n"); 
   pthread_t tid; 
   while(1){
       sin_size = sizeof other_adr; 
       new_fd = accept(socket_fd, (struct sockaddr *) &other_adr, &sin_size); 
       if(new_fd == -1){
           perror("accept"); 
           continue; 
       }
       NetworkRequestChannel* n = new NetworkRequestChannel(new_fd); 
       pthread_create (&tid, NULL, connection_handler,n); 
   }
}

NetworkRequestChannel::NetworkRequestChannel(int sfd){
    sockfd = sfd; 
}

string NetworkRequestChannel::send_request(string _request){
	cwrite(_request);
	string read = cread();
	return read;
}

string NetworkRequestChannel::cread(){
    char buf[1024]; 
    if(recv(sockfd, buf, sizeof(buf), 0) < 0){
        perror("recv"); 
        return ""; 
    }
    string msg = buf ; 
    return msg; 
}

int NetworkRequestChannel::cwrite(string _msg){
    if(send(sockfd, _msg.c_str(), _msg.size()+ 1, 0) == -1){
        perror("send"); 
        return -1; 
    }
    return 0; 
}

NetworkRequestChannel::~NetworkRequestChannel(){
    close(sockfd);
}

int NetworkRequestChannel::socket_fd(){
    return sockfd;
}