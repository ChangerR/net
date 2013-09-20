#ifndef __URL_FILE
#define __URL_FILE

typedef enum _URL_TYPE {
	URL_HTTP,URL_FTP,URL_FILE,URL_HTTPS,URL_FTPS
}URL_TYPE;
typedef struct _URL_CONNECTTION{
	URL_TYPE type;
	char  ip[16];
	unsigned short port;
	char f[128];
	char Host[120];
	char data[260];
}URL_CONNECT;

int URLAnalysis(const char* url,URL_CONNECT* url_conn);
int HttpDown(URL_CONNECT* pUrl,int method);
void show(URL_CONNECT* p);
int URL_Down(const char*url);
#endif