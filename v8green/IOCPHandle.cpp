#include "standard.h"

IOCPHandle::IOCPHandle(HANDLE handle): handle(handle) { }
IOCPHandle::~IOCPHandle() { CloseHandle(handle); }