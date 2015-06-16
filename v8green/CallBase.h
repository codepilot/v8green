#pragma once

typedef class CallBase {
private:
	//#include "ClassHeap.h"
public:
	typedef void (*PCBFUNC)(CallBase *obj, DWORD bytes, ULONG_PTR key);
	OVERLAPPED overlapped;
	PCBFUNC cb;
	LPVOID buffer;
	size_t bufferSize;
	CallBase();
	operator LPOVERLAPPED();
	static CallBase *cast(LPOVERLAPPED lpOverlaped) {
		return reinterpret_cast<pCallBase>(lpOverlaped);
	}
	static void dequeue(HANDLE CompletionPort, DWORD dwMilliseconds) {
		DWORD NumberOfBytes = 0;
		ULONG_PTR CompletionKey = 0;
		LPOVERLAPPED lpOverlapped = nullptr;
		BOOL ret = 0;
		mThrowFailureAllow({WAIT_TIMEOUT}, ret = GetQueuedCompletionStatus(CompletionPort, &NumberOfBytes, &CompletionKey, &lpOverlapped, dwMilliseconds));
		if(ret) {
			auto obj = cast(lpOverlapped);
			obj->cb(obj, NumberOfBytes, CompletionKey);
		}
	}
} *pCallBase;

static_assert(offsetof(CallBase, overlapped) == 0, "CallBase::overlapped must be at offset 0");