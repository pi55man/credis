#include <asm-generic/socket.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <endian.h>
#include<arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/ip.h>
using namespace std;

static void dummy(int connfd);

int main() {
  	int fd = socket(AF_INET,SOCK_STREAM,0);
	//af_inet is for ipv4. AF_INET6 is for ipv6.for simplicity, we consider ipv4.
	//sock_stream is for tcp. sock_Dgram is for udp, which we're not using.
	
	//to set socket options after creating the socket, use setsockopt()
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	//SOL_SOCKET specifies that so_reuseaddr is a socket layer option, not a protocol layer one.
	//SO_REUSEADDR  allows a socket to bind an address and port which is already in use, but prev socket
	//has not fully been released. without it, bind() will fail when you restart the server.
	
	//this holds an ipv4 address and port.
	//ntohs and ntohl convert the numbers in req big endian formats.
	struct sockaddr_in addr= {};
	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(1234);
	addr.sin_addr.s_addr = ntohl(0);
	int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
	if(rv){
	cout<<"bind()";
	}

	//listen
	rv = listen(fd,SOMAXCONN);
	if(rv){
		cout<<"listen()";
	}
	//the SOMAXCONN is the size of out queue in which established connections are placed, its defined as 128
	//on linux.
	
	//accept connections
	while(true){
	struct sockaddr_in client_addr ={};
	socklen_t addrlen = sizeof(client_addr);
	int connfd = accept(fd, (struct sockaddr* )&client_addr, &addrlen);
	//the accept syscallalso returns the peers address, addrlen is both input and output.
	
	if(connfd<0){continue;}
	dummy(connfd);
	close(connfd);
	}
	
	return 0;
}


static void dummy(int connfd){
	char rbuf[64]=  {};
	ssize_t n = read(connfd, &rbuf, sizeof(rbuf)-1);
	
	if(n<0){
	cout<<"read() error";
	return;
	}
	
	cout<<"client says: \n"<<rbuf;
	char wbuf[] = "world";
	write(connfd, wbuf, strlen(wbuf));
}
