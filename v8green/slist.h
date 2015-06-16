#pragma once

template <class T>
class slist {
public:
	//#include "ClassHeap.h"
	typedef struct _ENTRY {
		SLIST_ENTRY entry;
		T obj;
	} ENTRY, *PENTRY;
	SLIST_HEADER header;
	slist() { InitializeSListHead(&header); }
	slist(size_t prealloc) {
		InitializeSListHead(&header);
		for(size_t i = 0; i < prealloc; i++) {
			push();
		}
	}
	//static PENTRY make() { return (PENTRY)_aligned_malloc(sizeof(ENTRY),MEMORY_ALLOCATION_ALIGNMENT); }
	//static PENTRY make() { return hzalloc<ENTRY>(sizeof(ENTRY)); }
	static PENTRY make() {
		//createClassHeap();
		//_InterlockedIncrement64(&allocationCount);
		return (PENTRY)_aligned_malloc(sizeof(ENTRY),MEMORY_ALLOCATION_ALIGNMENT);
		//return static_cast<ENTRY*>(HeapAlloc(classHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, sizeof(ENTRY)));
	}
	static PENTRY make(T obj) { auto ret = make(); ret->obj = obj; return ret; }
	PENTRY flush() { return (PENTRY)InterlockedFlushSList(&header); }
	PENTRY pop() { return (PENTRY)InterlockedPopEntrySList(&header); }
	T popContext() {
		auto pEntry = pop();
		if(!pEntry) {return nullptr; }
		auto ret = pEntry->obj;
		//operator delete(pEntry);
		_aligned_free(pEntry);
		return ret;
	}
	void push(PENTRY entry) { InterlockedPushEntrySList(&header, &entry->entry); }
	void push(T obj) { push(make(obj)); }
	void push(ULONG_PTR obj) { push((T)obj); }
	void push() { push(make()); }
	USHORT depth() { return QueryDepthSList(&header); }
};
//#include "ClassHeapDef.h"
//ClassHeapDef(slist<PUMS_CONTEXT>)