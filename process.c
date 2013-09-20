#include <stdio.h>
#include <windows.h>
#include "Tlhelp32.h"
#include <tchar.h>
#include <string.h>
BOOL promoteProcessPrivileges(const TCHAR* newPrivileges)
{
	HANDLE tokenHandle;
	TOKEN_PRIVILEGES structTkp;
	//获得当前进程的access token句柄
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle) == FALSE)
	{
		return FALSE;
	}
	//查找newPrivileges参数对应的Luid，并将结果写入structTkp.Privileges[0]的Luid域中
	if(LookupPrivilegeValue(NULL, newPrivileges, &structTkp.Privileges[0].Luid) == FALSE){
		CloseHandle(tokenHandle);
		return FALSE;
	}
	//设置structTkp结构
	structTkp.PrivilegeCount = 1;
	structTkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//通知操作系统更改权限
	if(AdjustTokenPrivileges(tokenHandle, FALSE, &structTkp, sizeof(structTkp), NULL, NULL) == FALSE){
		CloseHandle(tokenHandle);
		return FALSE;
	}
	CloseHandle(tokenHandle);	
	return TRUE;
}

void OpenExefile(const TCHAR* str)
{
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION process_info;
	
	ZeroMemory(&startupinfo,sizeof(STARTUPINFO));
	startupinfo.cb = sizeof(STARTUPINFO);
	startupinfo.wShowWindow = SW_SHOWNORMAL;
	ZeroMemory(&process_info,sizeof(&process_info));
	CreateProcess(NULL,(char*)str,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startupinfo,&process_info);
}