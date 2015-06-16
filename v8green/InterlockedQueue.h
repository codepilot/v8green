#pragma once

template <class T>
class InterlockedQueue {
public:
	//#include "ClassHeap.h"
	typedef struct _ENTRY {
		SLIST_ENTRY entry;
		T* obj;
	} ENTRY, *PENTRY;
	SLIST_HEADER header;
	InterlockedQueue() { InitializeSListHead(&header); }
	InterlockedQueue(size_t prealloc) {
		InitializeSListHead(&header);
		for(size_t i = 0; i < prealloc; i++) {
			push();
		}
	}
	//static PENTRY make() { return hzalloc<ENTRY>(sizeof(ENTRY)); }
	static PENTRY make() {
		//createClassHeap();
		//_InterlockedIncrement64(&allocationCount);
		PENTRY ret
		//mThrowFailure(ret = static_cast<ENTRY*>(HeapAlloc(classHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, sizeof(ENTRY)));
		ret = _aligned_malloc(sizeof(ENTRY), MEMORY_ALLOCATION_ALIGNMENT);
		return ret;
	}
	static PENTRY make(T* obj) { auto ret = make(); ret->obj = obj; return ret; }
	//PENTRY flush() { return (PENTRY)InterlockedFlushSList(&header); }
	T* pop() {
		PENRY entry;
		mThrowFailure(entry = (PENTRY)InterlockedPopEntrySList(&header));
		if(!entry){ return nullptr; }
		auto ret = entry->obj;
		//hzfree(entry);
		_aligned_free(entry);
		return ret;
	}
	void push(PENTRY entry) {
		mThrowFailure(InterlockedPushEntrySList(&header, &entry->entry));
	}
	void push(T* obj) { push(make(obj)); }
	//void push(ULONG_PTR obj) { push((T)obj); }
	//void push() { push(make()); }
	USHORT depth() {
		USHORT ret;
		mThrowFailure(ret = QueryDepthSList(&header));
		return ret;
	}
};