#include "standard.h"

UmsThread::UmsThread(PUMS_COMPLETION_LIST completion, LPVOID param) {
	ThreadAttributes threadAttribs(completion);
	mThrowFailure(threadHandle = CreateRemoteThreadEx(GetCurrentProcess(), nullptr, 0, threadProc, param, 0, threadAttribs, nullptr));
}

UmsThread::~UmsThread() {
	if(threadHandle) {
		mThrowFailure(CloseHandle(threadHandle));
	}
}