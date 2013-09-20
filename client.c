#include <stdio.h>
#include <Winsock2.h>
#include <string.h>
#include "netfile.h"
#define PORT 47234
char _current_path[256];
void GetInputAddr(char* addr) {
	printf("Please Input server addr:");
	scanf_s("%s",addr,16);
}

void SendMsg(SOCKET s) {
	int nRunning = 1;
	static char buf[256];
	FILE* tmpf = NULL;
	gets_s(buf,256);
	recv(s,_current_path,256,0);
	while(nRunning) {
		memset(buf,0,256);
		printf("%s>",_current_path);
		gets_s(buf,256);
		if(*buf) {
			send(s,buf,256,0);
		}
		switch(*buf) {
		case 'u':
			if(!strncmp(buf,"upload ",7)) {
				UploadFile(s,buf+7,0);
			} else if(!strncmp(buf,"url ",4)) {
				recv(s,buf,16,0);
				printf("%s\n",buf);
			}
			break;
		case 'd':
			if(!strncmp(buf,"down ",5)) {
				DownFile(s);
			}else if(!strncmp(buf,"dir",3)&&!buf[4]) {
				if(tmpf)
					fclose(tmpf);
				tmpf = stdout;
				DownFile2(s,&tmpf);
				tmpf = NULL;
			}
			break;
		case 'l':
			if(!strncmp(buf,"ls",2)&&!buf[3]) {
				if(tmpf)
					fclose(tmpf);
				tmpf = stdout;
				DownFile2(s,&tmpf);
				tmpf = NULL;
			}
			break;
		default:
			break;
		}
		if(*buf=='e'&&!strcmp(buf,"exit")) {
			nRunning = 0;
		}
	}
}
int main(int argc,char ** argv) {
	WSADATA wsadata;
	char ip[16];
	SOCKET s;
	struct sockaddr_in sockaddr;
	int nSize;
	WSAStartup(MAKEWORD(2,2),&wsadata);
	GetInputAddr(ip);
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	sockaddr.sin_family = PF_INET;
	sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);
	sockaddr.sin_port = htons(PORT);
	nSize = connect(s,(SOCKADDR*)&sockaddr,sizeof(SOCKADDR));
	if(nSize != 0) {
		printf("link server failed\n");
		return 1;
	}
	nSize = 0x12345678;
	send(s,(char*)&nSize,4,0);
	SendMsg(s);
	closesocket(s);
	return 0;
}