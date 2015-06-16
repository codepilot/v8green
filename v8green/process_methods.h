#ifdef V8_DEFINE_METHOD
	//#define v8Function(name, body) void func_##name(const v8::FunctionCallbackInfo<v8::Value>& info) { UNREFERENCED_PARAMETER(info); 	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
 { body } return; }
	//#define v8Method(obj, name, body) void func_##obj_##name(const v8::FunctionCallbackInfo<v8::Value>& info);
	#define combineNames(obj) func_##obj
	#define v8Method(obj, name, body) void combineNames(obj)_##name(const v8::FunctionCallbackInfo<v8::Value>& info);
	#define v8MethodF64(obj, name, body) v8Method(obj, name, body)
	#define v8Copy(obj, name, srcObj)
#endif //V8_DEFINE_METHOD

#ifdef V8_DEFINE_METHOD_BODIES
	#define combineNames(obj) func_##obj
	//#define v8Function(name, body) void func_##name(const v8::FunctionCallbackInfo<v8::Value>& info) { UNREFERENCED_PARAMETER(info); 	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
 { body } return; }
	#define v8Method(obj, name, body) void combineNames(obj)_##name(const v8::FunctionCallbackInfo<v8::Value>& info) { UNREFERENCED_PARAMETER(info); 	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
 { body; } return; }
	#define v8MethodF64(obj, name, body) v8Method(obj, name, return scope.Close(v8::Number::New(static_cast<double>(body))))
	#define v8Copy(obj, name, srcObj)
#endif //V8_DEFINE_METHOD_BODIES

#ifdef V8_DEFINE_SYMBOLS
	#define combineNames(obj) func_##obj
	#define PersistentSymbol(name) auto symbol_##name = v8::Persistent<v8::String>::New(v8::String::NewFromOneByte(isolate, #name));
	#define DirectMethod(obj, name) PersistentSymbol(name) obj->Set(symbol_##name, v8::FunctionTemplate::New(combineNames(obj)_##name));
	#define v8Method(obj, name, body) DirectMethod(obj, name)
	#define v8MethodF64(obj, name, body) v8Method(obj, name, body)
	#define v8Copy(obj, name, srcObj) obj->Set(symbol_##name, v8::FunctionTemplate::New(combineNames(srcObj)_##name));
#endif //V8_DEFINE_SYMBOLS


v8Method(console, time, puts(info) )
v8Method(console, timeEnd, puts(info) )
v8Method(console, log, puts(info) )

v8Method(fs, readFile, CallReadFile::call(info) )
v8Method(fs, writeFile, CallWriteFile::call(info) )
v8Method(fs, unlink, CallUnlink::call(info) )
v8Method(fs, open, CallOpen::call(info) )
v8Method(fs, write, CallWrite::call(info) )
v8Method(fs, close, CallClose::call(info) )

v8Method(global, puts, puts(info) )

v8Method(global, setTimeout, CallSleep::call(info) )
v8Method(global, setImmediate, CallSetImmediate::call(info) )
v8Method(global, createSimpleBuffer, return scope.Close(createSimpleBuffer(info)) )
v8Copy(contextObject, createSimpleBuffer, global)
v8Copy(contextObject, setImmediate, global)

//const auto TEST_LowMemoryNotification = 0;
v8Method(process, LowMemoryNotification, v8::V8::LowMemoryNotification() )
v8MethodF64(process, CallWriteFileTotalCounter, static_cast<double>(CallWriteFile::totalCounter) / static_cast<double>(frequency.QuadPart))
v8Method(process, exit, ExitProcess((info.Length() > 0)?info[0]->Uint32Value():0))
v8MethodF64(process, tickCount, GetTickCount64());
v8Method(process, uptime, {
	LARGE_INTEGER counter;
	mThrowFailure(QueryPerformanceCounter(&counter));

	return v8::Number::New(static_cast<double>(counter.QuadPart - startCounter.QuadPart) / static_cast<double>(frequency.QuadPart));
})

/*
v8Method(process, exit, {
	func_process_getStats(info);
	getchar();
	if(info.Length() > 0) {
		ExitProcess(info[0]->Uint32Value());
	} else {
		ExitProcess(0);
	}
})
*/
v8Method(process, getStats, {
	v8::V8::LowMemoryNotification();

	v8::HeapStatistics heapStats;
	v8::V8::GetHeapStatistics(&heapStats);
	printf("total_heap_size:            %Iu\n", heapStats.total_heap_size());
	printf("total_heap_size_executable: %Iu\n", heapStats.total_heap_size_executable());
	printf("used_heap_size:             %Iu\n", heapStats.used_heap_size());
	printf("heap_size_limit:            %Iu\n", heapStats.heap_size_limit());


	mThrowFailure(HeapCompact(MemFile::classHeap, 0));
	//mThrowFailure(HeapCompact(CallBase::classHeap, 0));
	mThrowFailure(HeapCompact(CallReadFile::classHeap, 0));
	//CallSetImmediate::createClassHeap();
	mThrowFailure(HeapCompact(CallSetImmediate::classHeap, 0));
	//CallSleep::createClassHeap();
	mThrowFailure(HeapCompact(CallSleep::classHeap, 0));
	mThrowFailure(HeapCompact(CallUnlink::classHeap, 0));
	mThrowFailure(HeapCompact(CallWriteFile::classHeap, 0));
	//Critical::createClassHeap();
	mThrowFailure(HeapCompact(Critical::classHeap, 0));
//	mThrowFailure(HeapCompact(CriticalIsolate::classHeap, 0));
	//#ifdef USE_QUEUE_INTERLOCKED
	//	mThrowFailure(HeapCompact(InterlockedQueue::classHeap, 0));
	//#endif
	//slistUmsContext::createClassHeap();
	mThrowFailure(HeapCompact(slistUmsContext::classHeap, 0));
	//UmsCompletionList::createClassHeap();
	mThrowFailure(HeapCompact(UmsCompletionList::classHeap, 0));
	//UmsThread::createClassHeap();
	mThrowFailure(HeapCompact(UmsThread::classHeap, 0));
	//ThreadAttributes::createClassHeap();
	mThrowFailure(HeapCompact(ThreadAttributes::classHeap, 0));

	mThrowFailure(HeapValidate(MemFile::classHeap, 0, nullptr));
	//mThrowFailure(HeapValidate(CallBase::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(CallReadFile::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(CallSetImmediate::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(CallSleep::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(CallUnlink::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(CallWriteFile::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(Critical::classHeap, 0, nullptr));
//	mThrowFailure(HeapValidate(CriticalIsolate::classHeap, 0, nullptr));
	//#ifdef USE_QUEUE_INTERLOCKED
	//	mThrowFailure(HeapValidate(InterlockedQueue::classHeap, 0, nullptr));
	//#endif
	mThrowFailure(HeapValidate(slistUmsContext::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(UmsCompletionList::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(UmsThread::classHeap, 0, nullptr));
	mThrowFailure(HeapValidate(ThreadAttributes::classHeap, 0, nullptr));

	if(MemFile::allocationCount) {printf("MemFile::allocationCount: %I64d\n", MemFile::allocationCount);}
	//if(CallBase::allocationCount) {printf("CallBase::allocationCount: %I64d\n", CallBase::allocationCount);}
	if(CallReadFile::allocationCount) {printf("CallReadFile::allocationCount: %I64d\n", CallReadFile::allocationCount);}
	if(CallSetImmediate::allocationCount) {printf("CallSetImmediate::allocationCount: %I64d\n", CallSetImmediate::allocationCount);}
	if(CallSleep::allocationCount) {printf("CallSleep::allocationCount: %I64d\n", CallSleep::allocationCount);}
	if(CallUnlink::allocationCount) {printf("CallUnlink::allocationCount: %I64d\n", CallUnlink::allocationCount);}
	if(CallWriteFile::allocationCount) {printf("CallWriteFile::allocationCount: %I64d\n", CallWriteFile::allocationCount);}
	if(Critical::allocationCount) {printf("Critical::allocationCount: %I64d\n", Critical::allocationCount);}
	//if(CriticalIsolate::allocationCount) {CriticalIsolate::allocatedCount: %I64d\n", CriticalIsolate::allocationCount);}
	//#ifdef USE_QUEUE_INTERLOCKED
	//	if(InterlockedQueue::allocationCount) {printf("InterlockedQueue::allocatedCount: %I64d\n", InterlockedQueue::allocationCount);}
	//#endif
	if(slistUmsContext::allocationCount) {printf("slistUmsContext::allocationCount: %I64d\n", slistUmsContext::allocationCount);}
	if(UmsCompletionList::allocationCount) {printf("UmsCompletionList::allocationCount: %I64d\n", UmsCompletionList::allocationCount);}
	if(UmsThread::allocationCount) {printf("UmsThread::allocationCount: %I64d\n", UmsThread::allocationCount);}
	if(ThreadAttributes::allocationCount) {printf("ThreadAttributes::allocationCount: %I64d\n", ThreadAttributes::allocationCount);}
	if(simpleBufferCount) {printf("simpleBufferCount: %I64d\n", simpleBufferCount);}
})


#ifdef V8_DEFINE_SYMBOLS
	#undef v8Copy
	#undef v8MethodF64
	#undef v8Method
	#undef PersistentSymbol
	#undef DirectMethod
	#undef combineNames
#endif

#ifdef V8_DEFINE_METHOD_BODIES
	#undef v8Copy
	#undef v8MethodF64
	#undef v8Method
	#undef combineNames
#endif

#ifdef V8_DEFINE_METHOD
	#undef v8Copy
	#undef v8MethodF64
	#undef v8Method
	#undef combineNames
#endif
