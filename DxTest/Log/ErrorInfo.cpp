#include "ErrorInfo.h"
#include <Windows.h>
#include <stdio.h>


void PopupError(const char* szError,...)
{
	static char buf[1024];
	va_list va;
	va_start(va,szError);
	vsprintf_s(buf,1024,szError,va);
	va_end(va);
	::MessageBoxA(0,buf,"Error",0);
}

void PopupErrWithLastErr(unsigned int dwError)
{
	if (dwError!=0)
	{
		void* lpMsg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
						FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						dwError,
						MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						(char*)&lpMsg,
						0,NULL);
		static char szString[1024];
		sprintf_s(szString,1024,"Error! LastError=%d\nReason=%s",dwError,lpMsg);
		LocalFree(lpMsg);
		MessageBoxA(0,szString,"Error",0);
	}
}