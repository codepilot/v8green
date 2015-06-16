#include "standard.h"

ThreadAttributes::ThreadAttributes(PUMS_COMPLETION_LIST completion) {
	size_t listSize = 48;
	mThrowFailureAllow({ERROR_INSUFFICIENT_BUFFER}, InitializeProcThreadAttributeList(nullptr, 1, 0, &listSize));
	//list = hzalloc<_PROC_THREAD_ATTRIBUTE_LIST>(listSize);
	//createClassHeap();
	//_InterlockedIncrement64(&allocationCount); 
	//mThrowFailure(list = static_cast<PPROC_THREAD_ATTRIBUTE_LIST>(HeapAlloc(classHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, listSize)));
	list = (PPROC_THREAD_ATTRIBUTE_LIST)_aligned_malloc(listSize, MEMORY_ALLOCATION_ALIGNMENT);
	if(!list) {
		throw("List is zero");
	}
	mThrowFailure(InitializeProcThreadAttributeList(list, 1, 0, &listSize));
	createThreadAttributes.UmsVersion = UMS_VERSION;
	mThrowFailure(CreateUmsThreadContext(&createThreadAttributes.UmsContext));
	createThreadAttributes.UmsCompletionList = completion;
	mThrowFailure(UpdateProcThreadAttribute(list, 0, PROC_THREAD_ATTRIBUTE_UMS_THREAD, &createThreadAttributes, sizeof(UMS_CREATE_THREAD_ATTRIBUTES), nullptr, nullptr));
}
ThreadAttributes::~ThreadAttributes() {
	DeleteProcThreadAttributeList(list);
	_aligned_free(list);
	//operator delete(list);
	//hzfree(list);
}
ThreadAttributes::operator PPROC_THREAD_ATTRIBUTE_LIST() {
	return list;
}
ThreadAttributes::operator PUMS_CREATE_THREAD_ATTRIBUTES() {
	return &createThreadAttributes;
}
