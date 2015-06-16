	static INIT_ONCE initOnceHeap;
	static HANDLE classHeap;
	static volatile LONG64 allocationCount;
	static BOOL CALLBACK InitcreateClassHeapFunction (PINIT_ONCE InitOnce, PVOID Parameter, PVOID *lpContext) {
		mThrowFailure(classHeap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0, 0));
		mThrowFailure(HeapSetInformation(classHeap, HeapEnableTerminationOnCorruption, nullptr, 0));
		//const ULONG HEAP_LFH = 2ul;
		//ULONG HeapInformation = HEAP_LFH;
		//mThrowFailure(HeapSetInformation(classHeap, HeapCompatibilityInformation, &HeapInformation, sizeof(HeapInformation)));
		return true;
	}
	static void createClassHeap() {
		mThrowFailure(InitOnceExecuteOnce(&initOnceHeap, InitcreateClassHeapFunction, nullptr, nullptr));
	}
	void *operator new(size_t size) {
		createClassHeap();
		LPVOID ret;
		mThrowFailure(ret = HeapAlloc(classHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, size));
		_InterlockedIncrement64(&allocationCount);
		//printf("allocationCount %I64d\n", allocationCount);
		return ret;
	}
	void *operator new[](size_t size) {
		createClassHeap();
		LPVOID ret;
		mThrowFailure(ret = HeapAlloc(classHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, size));
		_InterlockedIncrement64(&allocationCount);
		//printf("allocationCount %I64d\n", allocationCount);
		return ret;
	}
	void operator delete( void *mem) {
		mThrowFailure(HeapFree(classHeap, 0, mem));
		_InterlockedDecrement64(&allocationCount);
	}
	void operator delete( void *mem, size_t size) {
		mThrowFailure(HeapFree(classHeap, 0, mem));
		_InterlockedDecrement64(&allocationCount);
	}
	void operator delete[]( void *mem) {
		mThrowFailure(HeapFree(classHeap, 0, mem));
		_InterlockedDecrement64(&allocationCount);
	}
	void operator delete[]( void *mem, size_t size) {
		mThrowFailure(HeapFree(classHeap, 0, mem));
		_InterlockedDecrement64(&allocationCount);
	}