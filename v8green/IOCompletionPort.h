#pragma once

template <class T>
class IOCompletionPort {
public:
	//#include "ClassHeap.h"
	HANDLE ioCompletionPort;
	IOCompletionPort() {
		mThrowFailure(ioCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, MAX_UMS_THREADS));
	}
	void addHandle(HANDLE ioHandle, ULONG_PTR completionKey = 0) {
		mThrowFailure(SetFileCompletionNotificationModes(ioHandle, FILE_SKIP_COMPLETION_PORT_ON_SUCCESS | FILE_SKIP_SET_EVENT_ON_HANDLE));
		mThrowFailure(CreateIoCompletionPort(ioHandle, ioCompletionPort, completionKey, 0));
	}
	/*
	void get(DWORD dwMilliseconds = INFINITE) {
		DWORD NumberOfBytes;
		ULONG_PTR CompletionKey;
		LPOVERLAPPED lpOverlapped;
		mThrowFailure(GetQueuedCompletionStatus(ioCompletionPort, &NumberOfBytes, &CompletionKey, &lpOverlapped, dwMilliseconds));
	}
	void post(DWORD dwNumberOfBytesTransferred, ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped) {
		mThrowFailure(PostQueuedCompletionStatus(ioCompletionPort, dwNumberOfBytesTransferred, dwCompletionKey, lpOverlapped));
	}
	*/
	/*
	T* getKey(DWORD dwMilliseconds = INFINITE) {
		DWORD NumberOfBytes;
		T* CompletionKey;
		LPOVERLAPPED lpOverlapped;
		mThrowFailure(GetQueuedCompletionStatus(ioCompletionPort, &NumberOfBytes, reinterpret_cast<PULONG_PTR>(&CompletionKey), &lpOverlapped, dwMilliseconds));
		return CompletionKey;
	}
	*/
	typedef void (*cbULONG_PTR)(ULONG_PTR);
	typedef struct _cb_val {
		cbULONG_PTR cb;
		ULONG_PTR val;
	} cb_val, *pcb_val;
	void getEmptyLambda(DWORD dwMilliseconds = INFINITE) {
		DWORD NumberOfBytes;
		LPOVERLAPPED lpOverlapped;
		ULONG_PTR upKey;
		//mThrowFailureAllow({ERROR_ABANDONED_WAIT_0}, GetQueuedCompletionStatus(ioCompletionPort, &NumberOfBytes, &upKey, &lpOverlapped, dwMilliseconds));
		auto status = GetQueuedCompletionStatus(ioCompletionPort, &NumberOfBytes, &upKey, &lpOverlapped, dwMilliseconds);
		if(!status) {
			auto lastError = GetLastError();
			mThrowFailureAllow({ERROR_ABANDONED_WAIT_0}, false);
			return;
		}
		pcb_val key = (pcb_val)upKey;
		if(key->cb){
			key->cb(key->val);
		}else{
			DebugBreak();
		}
		delete key;
	}
	void postEmptyLambda(cbULONG_PTR dwCompletionKey, ULONG_PTR val) {
		pcb_val key = new cb_val();
		key->cb = dwCompletionKey;
		key->val = val;
		ULONG_PTR upKey = (ULONG_PTR)(key);
		mThrowFailure(PostQueuedCompletionStatus(ioCompletionPort, 0, upKey, nullptr));
	}
	/*
	void postKey(T* dwCompletionKey) {
		mThrowFailure(PostQueuedCompletionStatus(ioCompletionPort, 0, reinterpret_cast<ULONG_PTR>(dwCompletionKey), nullptr));
	}
	*/
	~IOCompletionPort() {
		if(ioCompletionPort) {
			mThrowFailure(CloseHandle(ioCompletionPort));
		}
	}
};