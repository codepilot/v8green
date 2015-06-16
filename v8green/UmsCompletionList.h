#pragma once

class UmsCompletionList {
public:
	//#include "ClassHeap.h"
	PUMS_COMPLETION_LIST completionList;
	PUMS_SCHEDULER_ENTRY_POINT umsEntryPoint;
	UmsCompletionList(PUMS_SCHEDULER_ENTRY_POINT umsEntryPoint);
	~UmsCompletionList();
	PUMS_CONTEXT dequeue(DWORD WaitTimeOut = 0);
	void transferQueue(DWORD WaitTimeOut = 0);
	operator PUMS_COMPLETION_LIST();
	void enter();
	static BOOL isUsmThreadSuspended(PUMS_CONTEXT UmsThread);
	static BOOL isUsmThreadTerminated(PUMS_CONTEXT UmsThread);
};
//#include "ClassHeapDef.h"
//ClassHeapDef(UmsCompletionList)