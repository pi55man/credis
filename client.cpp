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
using namespace std;

const size_t k_max_msg = 4096;
static int32_t query(int fd, const char* text){
	uint32_t  len = (uint32_t)strlen(text);
	
	char wbuf[4+k_max_msg+1];
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4],text, len);
	write_all(fd, wbuf, 4+len);
}


int main(){
	int fd = socket(AF_INET, SOCK_STREAM,0);

	struct sockaddr_in addr = {};
	addr.sin_family=AF_INET;
	addr.sin_port=ntohs(1234);
	addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
	int rv = connect(fd, (const struct sockaddr*)&addr, sizeof(addr));
	
	char msg[] = "hello";
	cout<<"str len : "<<strlen(msg)<<endl;
	write(fd, msg, strlen(msg));
	char rbuf[64] = {};
	
	ssize_t n = read(fd, rbuf, sizeof(rbuf)-1);
	cout<<endl<<"server says: \n" << rbuf;
	close(fd);
	return 0;

}
