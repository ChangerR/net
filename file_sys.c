#include <windows.h>
#include <stdio.h>
#include "file_sys.h"
char _current_path[MAX_PATH];
char _env_path[MAX_PATH];
char space[MAX_PATH];
void Init_CurrentPath() {
	memset(space,' ',MAX_PATH);
	GetCurrentDirectory(MAX_PATH,_current_path);
	strncpy(_env_path,".",MAX_PATH);
}
void enum_dir2(char* path,FILE* f,int level,int all) {
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	char *tmp_path;
	tmp_path = (char*)malloc(MAX_PATH);
	GetCurrentDirectory(MAX_PATH,tmp_path);
	SetCurrentDirectory(path);
	hFind = FindFirstFile("*", &fd);
	if(hFind == INVALID_HANDLE_VALUE) {
		printf("Enum file Failed\n");
		return;
	}
	do {
		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			if(*fd.cFileName!='.') {
				fwrite(space,1,level*2>MAX_PATH?MAX_PATH:level*2,f);
				fprintf(f,"+%s\\\n",fd.cFileName);
				if(all)
					enum_dir2(fd.cFileName,f,level+1,all);
			}
		} else {
			fwrite(space,1,level*2>MAX_PATH?MAX_PATH:level*2,f);
			fprintf(f,"-%s size=%d byte\n",fd.cFileName,fd.nFileSizeLow);
		}
	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);
	CloseHandle(hFind);
	SetCurrentDirectory(tmp_path);
	free(tmp_path);
}

void EnumDir(char* path,FILE* f) {
	if(path==0||f==0)
		return;
	enum_dir2(path,f,1,1);
}

void EchoDir(char* path,FILE* f) {
	if(path==0||f==0)
		return;
	enum_dir2(path,f,1,0);
}

int  isFullPath(const char* path) {
	return path[1]==':'?1:0;
}