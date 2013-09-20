#ifndef __FILE_SYS
#define __FILE_SYS
#define SERVERTEMPNAME "SERVERTEMP"
#define CLIENTTEMPNAME "CLIENTTEMP"
void Init_CurrentPath();
void EnumDir(char* path,FILE* f);
void EchoDir(char* path,FILE* f);
int  isFullPath(const char* path);
extern  char _current_path[];
extern  char _env_path[];
#endif