#include <WinSock2.h>
#include <stdio.h>
#include "url.h"

void show(URL_CONNECT* p) {
	printf("URL TYPE: %d\n",p->type);
	printf_s("HOST NAME: %s\n",p->Host);
	printf_s("URL IP: %s\n",p->ip);
	printf("URL PORT: %u\n",p->port);
	printf("URL DATA %s\n",p->data);
	printf("URL f: %s\n\n",p->f);
}

int IsIpAddress(const char* ip) {
	int num = 0;
	int data_times = 0;
	while(*ip) {
		if (*ip >='0'&&*ip <='9') {
			num*=10;
			num+=*ip - '0';
			if (num > 255) {
				return 0;
			}
		} else if(*ip=='.') {
			data_times++;
			num = 0;
			if (data_times > 3) {
				return 0;
			}
		} else if (*ip==':'||*ip == '/') {
			return data_times==3?1:0;
		} else {
			return 0;
		}
		ip++;
	}
	return 1;
}

int URLAnalysis( const char* url,URL_CONNECT* url_conn ) {
	const char *p = url;
	static char host_name[256];
	char *ph = host_name;
	struct hostent* phost;
	if (url==NULL) {
		printf("url cannot be NULL\n");
		return 1;
	}
	while(*p==' '||*p == '\t')
		p++;
	switch(*p) {
	case 'h':
		if(!strncmp(p,"http",4)) {
			if (strncmp(p+4,"://",3)) {
				printf("http format error\n");
				return 1;
			}
			url_conn->type = URL_HTTP;
			p+=7;
		} else if (!strncmp(p,"https://",8)) {
			url_conn->type = URL_HTTPS;
			p+=8;
		} else {
			url_conn->type = URL_HTTP;
		}
		break;
	case 'f':
		if (!strncmp(p,"ftp://",6)) {
			url_conn->type = URL_FTP;
			p+=6;
		} else if (!strncmp(p,"file://",7)) {
			url_conn->type = URL_FILE;
			p+=7;
		} else if (!strncmp(p,"ftps://",7)) {
			url_conn->type = URL_FTPS;
			p+=7;
		} else {
			url_conn->type = URL_HTTP;
		}
		break;
	default:
		url_conn->type = URL_HTTP;
		break;
	}
	if (url_conn->type != URL_HTTP) {
		printf("just support http\n");
		return 1;
	}
	while(*p != ':'&&*p!='/'&&*p) {
		*ph++ = *p++;
	}
	*ph = 0;
	if (IsIpAddress(host_name)) {
		strncpy(url_conn->ip,host_name,16);
		strncpy(url_conn->Host,host_name,16);
	} else {
		phost = gethostbyname(host_name);
		if (phost == NULL) {
			printf("this host name is failed\n");
			return 1;
		}
		if (*phost->h_aliases) {
			strncpy(url_conn->Host,*phost->h_aliases,120);
		} else {
			strncpy(url_conn->Host,phost->h_name,120);
		}

		sprintf(url_conn->ip,"%d.%d.%d.%d",
		        (phost->h_addr_list[0][0]&0x00ff),
		        (phost->h_addr_list[0][1]&0x00ff),
		        (phost->h_addr_list[0][2]&0x00ff),
		        (phost->h_addr_list[0][3]&0x00ff));
	}
	url_conn->port = 80;
	if (*p == ':') {
		url_conn->port = 0;
		p++;
		while(*p!='/'&&*p) {
			if (*p >= '0'&& *p <= '9') {
				url_conn->port*=10;
				url_conn->port+=*p-'0';
			} else {
				printf("unconnect port format\n");
				return 1;
			}
			p++;
		}
	}
	if(*p == 0) {
		url_conn->f[0] = '/';
		url_conn->f[1] = 0;
	} else {
		url = p;
		p = url_conn->f;
		while(*url!='?'&&*url) {
			*(char*)p++ = *url++;
		}
		*(char*)p = 0;
		if (*url == '?') {
			strcpy(url_conn->data,url+1);
		} else {
			url_conn->data[0] = 0;
		}
	}
	return 0;
}

char request[] = "%s %s HTTP/1.1\r\n"
                 "Host:%s \r\n"
                 "Accept:*/* \r\n"
                 "Accept-Language:zh-cn \r\n"
                 "User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:23.0) Gecko/20100101 Firefox/23.0 \r\n"
                 "Connection:Keep-Alive \r\n"
                 "\r\n"
                 "%s";

typedef enum _HTTP_DOWN_STATE {
	READ_HEAD,READ_BODY
} READSTATE;

const char* GetFileName(const char* name) {
	const char* p = name;
	while(*p)
		++p;
	while (*p!='/'&&p!=name) {
		--p;
	}
	if (*p == '/') {
		p++;
	}
	return p;
}
int HttpDown(URL_CONNECT* pUrl,int method ) {
	SOCKET s;
	int nRet,nRead;
	SOCKADDR_IN addr;
	static char buf[1024];
	char method_str[12];
	int file_len;
	char* p;
	const char* filename;
	READSTATE state = READ_HEAD;
	FILE* f = NULL;
	URL_CONNECT* pRloc;
	static int fun_times;
	++fun_times;
	if (fun_times>5) {
		printf(
		    "Http down have be call 6 times\n");
		fun_times = 0;
		return 1;
	}
	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(pUrl->port);
	addr.sin_addr.S_un.S_addr = inet_addr(pUrl->ip);
	nRet = connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));

	if (nRet != 0) {
		printf("connect failed\n");
		nRet = 1;
		goto endp;
	}
	switch(method) {
	case 1:
		strcpy(method_str,"POST");
		break;
	default:
		strcpy(method_str,"GET");
	}
	sprintf_s(buf,1024,request,method_str,pUrl->f,pUrl->Host,pUrl->data);
	if (send(s,buf,strlen(buf),0) == SOCKET_ERROR) {
		printf("send data error\n");
		nRet = 1;
		goto endp;
	}

	while (1) {
		memset(buf,0,1024);
		if ((nRead = recv(s,buf,1024,0))==SOCKET_ERROR) {
			printf(
			    "recv data error\n");
			nRet = 1;
			goto endp;
		}
		p = buf;
		if (state == READ_HEAD) {
			if (strncmp(p,"HTTP/1.1 ",9)&&strncmp(p,"HTTP/1.0 ",9)) {
				printf("uncorrect stream format\n");
				nRet = 1;
				goto endp;
			}
			p += 9;
			file_len = 0;
			while(*p>='0'&&*p<='9') {
				file_len*=10;
				file_len+=*p-'0';
				++p;
			}
			if(file_len >= 300&&file_len<400) {
				while(*p&&p-buf<=1020) {
					if (*p == 'L'&&*(p+1)=='o') {
						if (!strncmp(p,"Location: ",10)) {
							p+=10;
							filename = p;
							while(*p!='\r')
								++p;
							*p = 0;
							pRloc = (URL_CONNECT*)malloc(sizeof(URL_CONNECT));
							URLAnalysis(filename,pRloc);
							printf("a relocation is find\n");
							show(pRloc);
							if (file_len == 307) {
								printf("error code 307\n");
								HttpDown(pRloc,1);
							} else {
								HttpDown(pRloc,0);
							}
							free(pRloc);
							nRet = 0;
							goto endp;
						}
					}
					++p;
				}
				printf("Relocation failed\n");
				nRet = 1;
				goto endp;
			} else if (file_len != 200) {
				printf("error code %d\n%s\n",file_len,p+1);
				nRet = 1;
				goto endp;
			}
			while(*p!='\n')
				p++;
			++p;
			while (*p) {
				if (*p == '\r'&&*(p+1)=='\n') {
					p+=2;
					break;
				} else if (*p == 'C'&&*(p+1) == 'o') {
					if (!strncmp(p,"Content-Length:",15)) {
						file_len = 0;
						p+=15;
						if (*p == ' ') {
							++p;
						}
						while(*p>='0'&&*p<='9') {
							file_len*=10;
							file_len+=*p-'0';
							++p;
						}
						printf("The All Down file size:%d\n",file_len);
					}
				}
				while(*p!='\n')
					p++;
				++p;
			}
			filename = GetFileName(pUrl->f);
			f = fopen(*filename==0?"index.html":filename,"wb");
			nRet = 0;
			if (p-buf<nRead) {
				fwrite(p,1,nRead-(p-buf),f);
				nRet=nRead-(p-buf);
			}
			state = READ_BODY;
		} else {
			if (nRead == 0) {
				if (nRet!=file_len) {
					printf("data size uncorrect\n");
				}
				break;
			}
			fwrite(p,1,nRead,f);
			printf("Have Recv Data :%d\n",nRead);
			nRet+=nRead;
			if(file_len&&nRet == file_len)
				break;
		}
	}
	nRet = 0;
endp:
	if (f) {
		printf("file have recv ok!\n");
		fclose(f);
	}
	--fun_times;
	closesocket(s);
	return nRet;
}

int URL_Down(const char*url) {
	URL_CONNECT* pUrl;
	int nRet;
	pUrl = (URL_CONNECT*)malloc(sizeof(URL_CONNECT));
	nRet = URLAnalysis(url,pUrl);
	if(nRet)
		goto ep;
	switch(pUrl->type) {
	case URL_HTTP:
		nRet = HttpDown(pUrl,0);
		break;
	default:
		break;
	}
ep:
	free(pUrl);
	return nRet;
}