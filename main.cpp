#include <asm-generic/socket.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <endian.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <cassert>


using namespace std;

static int32_t read_full(int fd, char* buf, size_t n);
static int32_t write_all(int fd, const char*buf, size_t n);
static int32_t one_request(int connfd);
const size_t k_max_msg = 4096;

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
	//bind
	struct sockaddr_in addr= {};
	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(1234);
	addr.sin_addr.s_addr = ntohl(0);
	int rv = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
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
	socklen_t socklen = sizeof(client_addr);
	int connfd = accept(fd, (struct sockaddr* )&client_addr, &socklen);
	//the accept syscall also returns the peers address, addrlen is both input and output.
	
	if(connfd<0){continue;}
	while(true){
		int32_t err = one_request(connfd);
			if(err){break;}
		}
	close(connfd);
	}
	//the one_request function parses only one request and responds, until connection is lost.
	return 0;
}


static int32_t read_full(int fd, char* buf, size_t n ){
	while(n>0){
		ssize_t rv= read(fd,buf,n);
		assert((size_t)rv <=n);
		n -= (size_t)rv;
		buf+=rv;
	};
	return 0;
}

static int32_t write_all(int fd, const char* buf, size_t n){
	while(n>0){
	ssize_t rv = write(fd, buf, n);
	assert((size_t)rv<=n);
	n-= (size_t)rv;
	buf+=rv;
	} 
	return 0; 
}
static int32_t one_request(int connfd){
	//4 bytes header
	
	char rbuf[4+k_max_msg+1];
	read_full(connfd, rbuf, 4);
	
	uint32_t len = 0;
	memcpy(&len, rbuf, 4);
	
	read_full(connfd, &rbuf[4],len);

	rbuf[4+len] = '\0';
	cout<<"client says: "<<&rbuf[4]<<flush;


	const char reply[] = "world";
	char wbuf[4+sizeof(reply)];
	len = (uint32_t)strlen(reply);
	memcpy(wbuf,&len, 4);
	memcpy(&wbuf[4],reply,len);
	return write_all(connfd,wbuf,len+4);
}
