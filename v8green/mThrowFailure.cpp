#include <Windows.h>
#include <stdio.h>
#include "mThrowFailure.h"

BOOL CheckSuccess(DWORD *allowedErrors, size_t numAllowed, BOOL success, const LPSTR timeStamp, const LPSTR fileInfo, const int line, const LPSTR func, const LPSTR callStr) {
	LPSTR lpMsgBuf;
	if(success) { return success; }
	DWORD dw = GetLastError();
	for(size_t i = 0; i < numAllowed; i++) {
		if(allowedErrors[i] == dw) {
			return success;
		}
	}
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&lpMsgBuf),
		0, nullptr);
	char lineDst[1000];
	sprintf_s(lineDst, "\n%s(%s) line %d\n  %s\n    %s\n      (%u)%s\n", fileInfo, timeStamp, line, func, callStr, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
	OutputDebugStringA(lineDst);
	DebugBreak();
	ExitProcess(1);
	return success;
}