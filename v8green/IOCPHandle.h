#pragma once

typedef class IOCPHandle {
	public:
	HANDLE handle;
	IOCPHandle(HANDLE handle);
	~IOCPHandle();
} *pIOCPHandle;
