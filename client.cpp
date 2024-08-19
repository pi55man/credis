#include <asm-generic/socket.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
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
#include <cassert>

using namespace std;

const size_t k_max_msg = 4096;
static int32_t write_all(int fd, const char* buf, size_t n);
static int32_t read_full(int fd, char* buf, size_t n);
static int32_t query(int fd, const char* text){
	uint32_t  len = (uint32_t)strlen(text);
	
	char wbuf[4+k_max_msg];
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4],text, len);
	write_all(fd, wbuf, 4+len);

	char rbuf[4+k_max_msg+1];
	read_full(fd, rbuf, 4);

	memcpy(&len, rbuf, 4);

	read_full(fd, &rbuf[4],len);
	rbuf[4+len]='\0';
	cout<<"server says: " <<&rbuf[4]<<flush;
	return 0; 
}

	
int main(){
	int fd = socket(AF_INET, SOCK_STREAM,0);

	struct sockaddr_in addr = {};
	addr.sin_family=AF_INET;
	addr.sin_port=ntohs(1234);
	addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
	int rv = connect(fd, (const struct sockaddr*)&addr, sizeof(addr));
	int32_t err = query(fd,"hello");
	if(err){close(fd);}
	err = query(fd,"hello2");
	if(err){close(fd);}
	err =query(fd,"hello3");
	if(err){close(fd);}
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

static int32_t read_full(int fd, char* buf, size_t n){
	while(n<0){
	ssize_t rv = read(fd, buf, n);
	assert((size_t)rv<=n);
		n-=(size_t)rv;
	buf+=rv;
	}
	return 0;
}

