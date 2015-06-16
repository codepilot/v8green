#pragma once

#include "mThrowFailure.h"

class MemFile {
public:
	//#include "ClassHeap.h"
	HANDLE fileHandle;
	HANDLE mappingHandle;
	LPVOID filePtr;
	LONGLONG volatile references;
public:
	inline void incRef() {
		InterlockedIncrement64(&references);
	}
	inline void decRef(){
		if(InterlockedDecrement64(&references) <= 0){
			delete this;
		}
	}
	LONGLONG fileSize;
	MemFile(HANDLE iFileHandle, HANDLE iMappingHandle, LPVOID iFilePtr);
	MemFile(LPCWSTR lpFileName, DWORD dwDesiredAccess = GENERIC_READ, DWORD flProtect = PAGE_READONLY, DWORD dwDesiredPtrAccess = FILE_MAP_READ, DWORD dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr, DWORD dwCreationDisposition = OPEN_EXISTING, DWORD dwFlagsAndAttributes = 0, HANDLE hTemplateFile = nullptr);
	MemFile(LPCSTR lpFileName, DWORD dwDesiredAccess = GENERIC_READ, DWORD flProtect = PAGE_READONLY, DWORD dwDesiredPtrAccess = FILE_MAP_READ, DWORD dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr, DWORD dwCreationDisposition = OPEN_EXISTING, DWORD dwFlagsAndAttributes = 0, HANDLE hTemplateFile = nullptr);
	~MemFile();
	inline char* operator*(){
		return (char*)filePtr;
	}
	inline operator const char *() {
		return (const char *)filePtr;
	}
};