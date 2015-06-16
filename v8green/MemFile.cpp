#pragma warning(disable: 4100 4127 4189 4267 4350 4355 4365 4510 4514 4610 4706 4710 4820 4946 4668 4820 4668)

#pragma warning(push)
#include "targetver.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#pragma warning(pop)

#include "MemFile.h"

//#include "ClassHeapDef.h"
//ClassHeapDef(MemFile)


MemFile::MemFile(HANDLE iFileHandle, HANDLE iMappingHandle, LPVOID iFilePtr) {
	references = 1;
	//printf("MemFile()\n");
	fileHandle = iFileHandle;
	mappingHandle = iMappingHandle;
	filePtr = iFilePtr;
	LARGE_INTEGER liFileSize = {0,0};
	mThrowFailure(GetFileSizeEx(fileHandle, &liFileSize));
	fileSize = liFileSize.QuadPart;
}

MemFile::MemFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD flProtect, DWORD dwDesiredPtrAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	references = 1;
	//wprintf(L"MemFile(%s)", lpFileName);
	mThrowFailure(fileHandle = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile));
	//printf("=%I64x\n", fileHandle);
	mThrowFailure(mappingHandle = CreateFileMapping(fileHandle, nullptr, flProtect, 0, 0, nullptr));
	if(mappingHandle) {
		mThrowFailure(filePtr = MapViewOfFile(mappingHandle, dwDesiredPtrAccess, 0, 0, 0));
	} else {
		filePtr = nullptr;
	}
	LARGE_INTEGER liFileSize = {0,0};
	mThrowFailure(GetFileSizeEx(fileHandle, &liFileSize));
	fileSize = liFileSize.QuadPart;
}

MemFile::MemFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD flProtect, DWORD dwDesiredPtrAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	references = 1;
	//wprintf(L"MemFile(%s)", lpFileName);
	mThrowFailure(fileHandle = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile));
	//printf("=%I64x\n", fileHandle);
	mThrowFailure(mappingHandle = CreateFileMapping(fileHandle, nullptr, flProtect, 0, 0, nullptr));
	if(mappingHandle) {
		mThrowFailure(filePtr = MapViewOfFile(mappingHandle, dwDesiredPtrAccess, 0, 0, 0));
	} else {
		filePtr = nullptr;
	}
	LARGE_INTEGER liFileSize = {0,0};
	mThrowFailure(GetFileSizeEx(fileHandle, &liFileSize));
	fileSize = liFileSize.QuadPart;
}
MemFile::~MemFile() {
	//printf("~MemFile(%I64x)\n", fileHandle);
	mThrowFailure(CloseHandle(fileHandle));
	mThrowFailure(CloseHandle(mappingHandle));
	mThrowFailure(UnmapViewOfFile(filePtr));
}