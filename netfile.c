#include <Winsock2.h>
#include <stdio.h>
#include "netfile.h"
#define STREAM_HEAD 8
#define MAX_FILEBUF 1024
static unsigned char filebuf[MAX_FILEBUF];

void UploadFile(SOCKET s,char* filename,unsigned int key) {
	FILE* f;
	f = fopen(filename,"rb");
	UploadFile2(s,f,filename,key);
	if(f)
		fclose(f);
}

void DownFile(SOCKET s) {
	FILE* f;
	f = NULL;
	DownFile2(s,&f);
	printf("DownFile2 Ok\n");
	if(f)
		fclose(f);
}
int RecvAllData_s(SOCKET s,char* buf,int len) {
	int nRead;
	char* p = buf;
	while(len) {
		nRead = recv(s,p,len,0);
		if(nRead == SOCKET_ERROR)
		{
			printf("recv data error\n");
			return 1;
		}
		p += nRead;
		len -= nRead;
	}
	return 0;
}
void UploadFile2(SOCKET s,FILE* file,const char* filename,unsigned int key) {
	NETFILE netfile = {0};
	size_t len;
	const char *p = filename;
	while(*p)p++;
	while(*p!='/'&&*p!='\\'&&p!=filename)--p;
	if(*p=='/'||*p=='\\')++p;
	if(file == NULL) {
		netfile.type = NETFILE_END;
		send(s,(char*)&netfile,sizeof(NETFILE),0);
		printf("Can not find this file\n");
		return;
	}
	fseek(file,0,SEEK_END);
	netfile.len = ftell(file);
	fseek(file,0,SEEK_SET);
	netfile.type = NETFILE_TYPE;
	strncpy(netfile.filename,p,120);
	netfile.key = key;
	send(s,(char*)&netfile,sizeof(NETFILE),0);
	do {
		len = fread(filebuf+STREAM_HEAD,1,MAX_FILEBUF-STREAM_HEAD,file);
		netfile.len -= len;
		*(unsigned int*)filebuf = NETFILE_STREAM;
		*(int*)(filebuf+4) = len;
		send(s,filebuf,MAX_FILEBUF,0);
	} while(netfile.len > 0);
	netfile.type = NETFILE_END;
	send(s,(char*)&netfile,sizeof(NETFILE),0);
}
#define RecvAllData(a,b,c) if(RecvAllData_s(a,b,c))return;
void DownFile2(SOCKET s,FILE** file) {
	FILE* f;
	NETFILE netfile;
	
	RecvAllData(s,(char*)&netfile,sizeof(NETFILE));
	if(netfile.type != NETFILE_TYPE) {
		printf("error net file format or this file is not exsit in server\n");
		return;
	}
	if(file == NULL) {
		f = stderr;
		fprintf(f,"NULL FILE POINTER AND THE recive data will be output in stderr\n");
	}else
		f = *file;
	if(f == NULL) {
		if(*netfile.filename==0) {
			strcpy(netfile.filename,"temp");
		}
		f = fopen(netfile.filename,"wb");
		*file = f;
	}

	do {
		RecvAllData(s,filebuf,STREAM_HEAD);
		if(*(unsigned int*)filebuf == NETFILE_END) {
			RecvAllData(s,filebuf,sizeof(NETFILE)-STREAM_HEAD);
			break;
		}
		RecvAllData(s,filebuf+STREAM_HEAD,MAX_FILEBUF-STREAM_HEAD);
		fwrite(filebuf+STREAM_HEAD,1,*(int*)(filebuf+4),f);
	} while(1);
	if(f!=stdout&&f!=stdin&&f!=stderr&&netfile.len != ftell(f))
		printf("file length not correct \n");
}