#ifndef __NETFIEL
#define __NETFIEL
#define NETFILE_TYPE 0x12344321
#define NETFILE_END  0x56AC4B23
#define NETFILE_STREAM 0xAB23FF11
typedef struct _netfile {
	unsigned int type;
	char filename[120];
	int len;
	unsigned int key;
}NETFILE;
typedef struct file_header {
	unsigned int type;
	int length;
}FILEHEADER;
void DownFile(SOCKET s);
void UploadFile(SOCKET s,char* filename,unsigned int key);
void UploadFile2(SOCKET s,FILE* file,const char* filename,unsigned int key);
void DownFile2(SOCKET s,FILE** file);
#endif