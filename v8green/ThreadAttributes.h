#pragma once

class ThreadAttributes {
public:
	//#include "ClassHeap.h"
	PPROC_THREAD_ATTRIBUTE_LIST list;
	UMS_CREATE_THREAD_ATTRIBUTES createThreadAttributes;
	ThreadAttributes(PUMS_COMPLETION_LIST completion);
	~ThreadAttributes();
	operator PPROC_THREAD_ATTRIBUTE_LIST();
	operator PUMS_CREATE_THREAD_ATTRIBUTES();
};