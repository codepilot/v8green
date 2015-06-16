#pragma once

class UmsThread {
public:
	//#include "ClassHeap.h"
	HANDLE threadHandle;
	UmsThread(PUMS_COMPLETION_LIST completion, LPVOID param);
	~UmsThread();
};

//#include "ClassHeapDef.h"
//ClassHeapDef(UmsThread)
