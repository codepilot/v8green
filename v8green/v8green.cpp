#include "standard.h"

volatile uint64_t outStandingCallbacks = 0;
volatile uint64_t spoolingThreads = 0;
Critical iocpHandleProtection;

#ifdef USE_QUEUE_STD
Critical csCallQ;
#endif //USE_QUEUE_STD

#ifdef USE_QUEUE_IOCP
//#include "IOCompletionPort.h"
#endif //USE_QUEUE_IOCP

#ifdef USE_QUEUE_INTERLOCKED
//#include "InterlockedQueue.h"
#endif//USE_QUEUE_INTERLOCKED

#ifdef USE_QUEUE_IOCP
	//IOCompletionPort<CallBase> CallBase::iocpCall;
	//typedef void EmptyLambda();
	IOCompletionPort<EmptyLambda> iocpCall;
#endif //USE_QUEUE_IOCP


#ifdef USE_QUEUE_STD
	std::queue<CallBase *> CallBase::qCall;
#endif //USE_QUEUE_STD

#ifdef USE_QUEUE_INTERLOCKED
	InterlockedQueue<CallBase> CallBase::iqCall;
#endif//USE_QUEUE_INTERLOCKED


pV8Environment v8Environment;

pCriticalIsolate csIsolate;

HANDLE stdout_handle;

slistUmsContext contextList;
slistUmsContext contextSuspendedList;
#ifdef USE_QUEUE_STD
#ifdef UMS_YIELD_RESUME
slistUmsContext contextYieldList;
#endif
#endif

void NTAPI entryPoint(__in RTL_UMS_SCHEDULER_REASON Reason, __in ULONG_PTR ActivationPayload, __in PVOID SchedulerParam);
UmsCompletionList umsCompletionList(entryPoint);
//PUMS_COMPLETION_LIST completionList = 0;
CHAR* reasons[3] = {"Startup", "ThreadBlocked", "ThreadYield"};

#define writeFileFormatted1(fmt, arg1) \
	{\
		CHAR dst[1000];\
		size_t charsWritten = sprintf_s(dst, fmt, arg1);\
		DWORD bytesWritten = 0;\
		BOOL ret = WriteFile(stdout_handle, dst, (DWORD)charsWritten * (DWORD)sizeof(CHAR), &bytesWritten, nullptr);\
	}

#define OutputDebugStringFormatted(fmt, arg1) \
	{\
		CHAR dst[1000];\
		size_t charsWritten = sprintf_s(dst, fmt, arg1);\
		OutputDebugStringA(dst);\
	}

#define writeFileCHARS(dst, charsWritten) \
	{\
		DWORD bytesWritten = 0;\
		BOOL ret = WriteFile(stdout_handle, dst, (DWORD)charsWritten * (DWORD)sizeof(CHAR), &bytesWritten, nullptr);\
	}

#ifdef USE_QUEUE_STD
HANDLE startedEvent;
#endif

DWORD WINAPI threadProc(__in  LPVOID lpParameter){
	//if(0 == lpParameter) {
	auto remainingThreads = _InterlockedDecrement(&spoolingThreads);
	if(0 == remainingThreads) {
		csIsolate->protectedScope([](){
			//e->isolate->Enter();
			//	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

			auto result = v8Environment->persistent_script->Run();
			_InterlockedDecrement(&outStandingCallbacks);
		});
		#ifdef USE_QUEUE_STD
			mThrowFailure(SetEvent(startedEvent));
		#endif
	}
	#ifdef USE_QUEUE_STD
		mThrowFailure(WaitForSingleObject(startedEvent, INFINITE));
	#endif
	while(outStandingCallbacks) {
		#ifdef USE_QUEUE_STD
			//if(qCall.empty()) { Sleep(100); continue; }
			if(CallBase::qCall.empty()) {
				mThrowFailure(UmsThreadYield((PVOID)lpParameter));
				continue;
			}
			pCallBase n;
			csCallQ.protect([&n](){
				n = CallBase::qCall.front();
				CallBase::qCall.pop();
			});
		#endif
		#if 0
			#ifdef USE_QUEUE_IOCP
				auto n = CallBase::iocpCall.getKey();
			#endif
			#ifdef USE_QUEUE_INTERLOCKED
				auto n = CallBase::iqCall.pop();
				if(!n) {
					mThrowFailure(UmsThreadYield((PVOID)lpParameter));
					continue;
				}
			#endif

			//OutputDebugStringFormatted("threadProc(%d) wake\n", lpParameter);
			//printf("qCall %d\n", n->typeNum);
			switch(n->typeNum) {
				case CallBase::stypeNum: {
					auto nCallBase = static_cast<pCallBase>(n);
					printf("CallBase(%d)\n", nCallBase->typeNum);
					break;}
				case CallSetImmediate::stypeNum: {
					static_cast<pCallSetImmediate>(n)->call();
					break;}
				case CallSleep::stypeNum: {
					static_cast<pCallSleep>(n)->call();
					//auto nCallSleep = static_cast<pCallSleep>(n);
					//printf("CallSleep(%d)\n", nCallSleep->typeNum);
					//nCallSleep->call();
					break;}
				//case CallPuts::stypeNum: {
				//	auto nCallPuts = static_cast<pCallPuts>(n);
				//	//printf("CallPuts(%d)\n", nCallPuts->typeNum);
				//	nCallPuts->call();
				//	break;}
				case CallWriteFile::stypeNum: {
					auto nCallWriteFile = static_cast<pCallWriteFile>(n);
					//printf("CallWriteFile(%d)\n", nCallWriteFile->typeNum);
					nCallWriteFile->call();
					break;}
				case CallReadFile::stypeNum: {
					auto nCallReadFile = static_cast<pCallReadFile>(n);
					//printf("CallReadFile(%d)\n", nCallReadFile->typeNum);
					nCallReadFile->call();
					break;}
				case CallUnlink::stypeNum: {
					auto nCallUnlink = static_cast<pCallUnlink>(n);
					//printf("CallUnlink(%d)\n", nCallUnlink->typeNum);
					nCallUnlink->call();
					break;}
				case CallOpen::stypeNum: {
					auto nCallOpen = static_cast<pCallOpen>(n);
					//printf("CallOpen(%d)\n", nCallOpen->typeNum);
					nCallOpen->call();
					break;}
				case CallWrite::stypeNum: {
					auto nCallWrite = static_cast<pCallWrite>(n);
					//printf("CallWrite(%d)\n", nCallWrite->typeNum);
					nCallWrite->call();
					break;}
				case CallClose::stypeNum: {
					auto nCallClose = static_cast<pCallClose>(n);
					//printf("CallClose(%d)\n", nCallClose->typeNum);
					nCallClose->call();
					break;}
				default:
					puts("unknown");
			}
		#else
			iocpCall.getEmptyLambda();
		#endif
		//SleepEx(INFINITE, true);
		//OutputDebugStringFormatted("threadProc(%d) wake\n", lpParameter);
	}
	iocpHandleProtection.enter();
	if(iocpCall.ioCompletionPort) {
		CloseHandle(iocpCall.ioCompletionPort);
		iocpCall.ioCompletionPort = 0;
	}
	iocpHandleProtection.leave();
	//writeFileFormatted1("threadProc(%d) end\n", lpParameter);
	return 0;
}

bool insideCallback = false;
int insideCallbackThreadNum;

LARGE_INTEGER startCounter;
LARGE_INTEGER frequency;

#if 0
#define v8Function(name, body) void func_##name(const v8::FunctionCallbackInfo<v8::Value>& info) { UNREFERENCED_PARAMETER(info); 	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
 { body } return; }
#define v8Method(obj, name, body) void func_##obj_##name(const v8::FunctionCallbackInfo<v8::Value>& info) { UNREFERENCED_PARAMETER(info); 	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
 { body } return; }
typedef void HandleValue;
//typedef const v8::Arguments& Args;
#endif

#if 0
void CallWriteFileTotalCounter(const v8::FunctionCallbackInfo<v8::Value>& info) {
	UNREFERENCED_PARAMETER(args);
	return v8::Number::New(static_cast<double>(CallWriteFile::totalCounter) / static_cast<double>(frequency.QuadPart));
}
#endif

void puts(const v8::FunctionCallbackInfo<v8::Value>& info) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	for(int i = 0; i < info.Length(); i++) {
		if(i > 0) { fputs(", ", stdout); }
		fputws((wchar_t *)v8::String::Value(info[i]).operator*(), stdout);
	}
	puts("");
	return;
}

volatile LONG64 simpleBufferCount = 0;
void freeSimpleBuffer(v8::Persistent<v8::Value> object, void* parameter) {
	UNREFERENCED_PARAMETER(parameter);
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	auto len = object->ToObject()->GetIndexedPropertiesExternalArrayDataLength();
	auto buf = object->ToObject()->GetIndexedPropertiesExternalArrayData();
	v8::V8::AdjustAmountOfExternalAllocatedMemory(-len);
	_InterlockedDecrement64(&simpleBufferCount);
	mThrowFailure(VirtualFree(buf, 0, MEM_RELEASE));
	//printf("freeSimpleBuffer   %d = %p\n", len, parameter);
	object.Dispose();
}

void createSimpleBuffer(void *buf, size_t len) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	//printf("createSimpleBuffer %d = %p\n", len, buf);
	v8::V8::AdjustAmountOfExternalAllocatedMemory(len);
	auto ret = v8::Object::New();
	ret->Set(v8::String::NewFromOneByte(isolate, "length"), v8::Uint32::NewFromUnsigned(static_cast<uint32_t>(len)));
	v8::Persistent<v8::Object> pobj = v8::Persistent<v8::Object>::New(ret);
	pobj.MakeWeak(buf, freeSimpleBuffer);
	ret->SetIndexedPropertiesToExternalArrayData(buf, v8::ExternalArrayType::kExternalUnsignedByteArray, static_cast<int32_t>(len));
	_InterlockedIncrement64(&simpleBufferCount);
	return scope.Close(ret);
}

void createSimpleBuffer(size_t len) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	PVOID buf;
	mThrowFailure(buf = VirtualAlloc(nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	return scope.Close(createSimpleBuffer(buf, len));
}

void createSimpleBuffer(const v8::FunctionCallbackInfo<v8::Value>& info) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	if(info.Length() < 1) { return; }
	if(!info[0]->IsUint32()) { return; }
	return scope.Close(createSimpleBuffer(info[0]->Uint32Value()));
}

#undef writeFileCHARS
#define writeFileCHARS(dst, charsWritten)

auto contextCount = 0;
void makeThread() {
	static auto threadNum = 0;
	contextCount++;
	//contextFreeList.push();
	UmsThread ut(umsCompletionList, (LPVOID)threadNum);
	threadNum++;
}

void NTAPI entryPoint(__in RTL_UMS_SCHEDULER_REASON Reason, __in ULONG_PTR ActivationPayload, __in PVOID SchedulerParam) {
	UNREFERENCED_PARAMETER(SchedulerParam);
	switch(Reason){
	case RTL_UMS_SCHEDULER_REASON::UmsSchedulerThreadYield:
		#ifdef USE_QUEUE_STD
		#ifdef UMS_YIELD_RESUME
			contextYieldList.push(ActivationPayload);//inspect SchedulerParam
		#endif
		#endif
		break;
	case RTL_UMS_SCHEDULER_REASON::UmsSchedulerStartup:
		_InterlockedIncrement(&outStandingCallbacks);
		for(int n = 0; n < MAX_UMS_THREADS; n++){
			_InterlockedIncrement(&spoolingThreads);
		}
		for(int n = 0; n < MAX_UMS_THREADS; n++){
			makeThread();
		}
		break;
	case RTL_UMS_SCHEDULER_REASON::UmsSchedulerThreadBlocked:
		break;
	default:
		throw(Reason);
	};
	BOOL success;

	PUMS_CONTEXT nextThread = 0;
	for(;;){
		BOOL listNotEmpty = false;

		umsCompletionList.transferQueue();

		do{
			//pEntryUmsContext contextEntry;
			PUMS_CONTEXT pContext = nullptr;
			while(pContext = contextList.popContext()) {
				nextThread = pContext;
				//charsWritten = sprintf_s(dst, "contextEntry popped %p\n", nextThread);
				//writeFileCHARS(dst, charsWritten);
				if(UmsCompletionList::isUsmThreadSuspended(nextThread)){
					//charsWritten = sprintf_s(dst, "context suspended %p\n", nextThread);
					//writeFileCHARS(dst, charsWritten);
					contextSuspendedList.push(pContext);
				}else if(UmsCompletionList::isUsmThreadTerminated(nextThread)){
					contextCount--;
					//charsWritten = sprintf_s(dst, "context terminated %p, %d remain\n", nextThread, contextCount);
					//writeFileCHARS(dst, charsWritten);
					mThrowFailure(DeleteUmsThreadContext(pContext));
					//_aligned_free(contextEntry);
					//hzfree(contextEntry);
				}else{
					//contextFreeList.push(contextEntry);
					for(;;){
						//charsWritten = sprintf_s(dst, "ExecuteUmsThread %p\n", nextThread);
						//writeFileCHARS(dst, charsWritten);
						success = ExecuteUmsThread(nextThread);
						if(!success){
							DWORD le = GetLastError();
							if(ERROR_RETRY == le){
								//charsWritten = sprintf_s(dst, "ERROR_RETRY\n");
								//writeFileCHARS(dst, charsWritten);
								continue;
							}else if(ERROR_INVALID_PARAMETER == le){
								//charsWritten = sprintf_s(dst, "ERROR_INVALID_PARAMETER\n");
								//writeFileCHARS(dst, charsWritten);
								if(UmsCompletionList::isUsmThreadSuspended(nextThread)){
									//charsWritten = sprintf_s(dst, "context suspended LATE\n");
									//writeFileCHARS(dst, charsWritten);
								}else if(UmsCompletionList::isUsmThreadTerminated(nextThread)){
									//charsWritten = sprintf_s(dst, "context terminated LATE\n");
									//writeFileCHARS(dst, charsWritten);
									DeleteUmsThreadContext(pContext);
									//_aligned_free(contextEntry);
									//hzfree(contextEntry);
								}else{
									//charsWritten = sprintf_s(dst, "context error skip\n");
									//writeFileCHARS(dst, charsWritten);
								}
								break;
							}else{
								//charsWritten = sprintf_s(dst, "ExecuteUmsThread error %u\n", le);
								//writeFileCHARS(dst, charsWritten);
								//	success = DeleteUmsThreadContext(context);
								break;
							}
						}
					}
				}
			}
			while(pContext = contextSuspendedList.popContext()){
				//charsWritten = sprintf_s(dst, "TrySuspended\n");
				//writeFileCHARS(dst, charsWritten);
				contextList.push(pContext);
				listNotEmpty = true;
			}

			#ifdef USE_QUEUE_STD
				#ifdef UMS_YIELD_RESUME
					if(!outStandingCallbacks || !CallBase::qCall.empty()) {
						while(pContext = contextYieldList.popContext()){
							//charsWritten = sprintf_s(dst, "TryYielded %s %s\n", !outStandingCallbacks?"!outStandingCallbacks":"", !qCall.empty()?"!qCall.empty()":"");
							//writeFileCHARS(dst, charsWritten);
							contextList.push(pContext);
							listNotEmpty = true;
						}
					}
				#endif
			#endif
		}while(listNotEmpty);

		if(!contextCount) {
			//CHAR dst[1000];
			//auto charsWritten = sprintf_s(dst, "!contextCount\n");
			//writeFileCHARS(dst, charsWritten);
			break;
		}
		umsCompletionList.transferQueue(INFINITE);
	}
	return;
}

void ums_init() {
	mThrowFailure(stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE));
	umsCompletionList.enter();
}

#if 0
void process_getStats(const v8::FunctionCallbackInfo<v8::Value>& info) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);


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
	CallSetImmediate::createClassHeap();
	mThrowFailure(HeapCompact(CallSetImmediate::classHeap, 0));
	CallSleep::createClassHeap();
	mThrowFailure(HeapCompact(CallSleep::classHeap, 0));
	mThrowFailure(HeapCompact(CallUnlink::classHeap, 0));
	mThrowFailure(HeapCompact(CallWriteFile::classHeap, 0));
	Critical::createClassHeap();
	mThrowFailure(HeapCompact(Critical::classHeap, 0));
//	mThrowFailure(HeapCompact(CriticalIsolate::classHeap, 0));
	#ifdef USE_QUEUE_INTERLOCKED
		mThrowFailure(HeapCompact(InterlockedQueue::classHeap, 0));
	#endif
	slistUmsContext::createClassHeap();
	mThrowFailure(HeapCompact(slistUmsContext::classHeap, 0));
	UmsCompletionList::createClassHeap();
	mThrowFailure(HeapCompact(UmsCompletionList::classHeap, 0));
	UmsThread::createClassHeap();
	mThrowFailure(HeapCompact(UmsThread::classHeap, 0));
	ThreadAttributes::createClassHeap();
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
	#ifdef USE_QUEUE_INTERLOCKED
		mThrowFailure(HeapValidate(InterlockedQueue::classHeap, 0, nullptr));
	#endif
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
	#ifdef USE_QUEUE_INTERLOCKED
		if(InterlockedQueue::allocationCount) {printf("InterlockedQueue::allocatedCount: %I64d\n", InterlockedQueue::allocationCount);}
	#endif
	if(slistUmsContext::allocationCount) {printf("slistUmsContext::allocationCount: %I64d\n", slistUmsContext::allocationCount);}
	if(UmsCompletionList::allocationCount) {printf("UmsCompletionList::allocationCount: %I64d\n", UmsCompletionList::allocationCount);}
	if(UmsThread::allocationCount) {printf("UmsThread::allocationCount: %I64d\n", UmsThread::allocationCount);}
	if(ThreadAttributes::allocationCount) {printf("ThreadAttributes::allocationCount: %I64d\n", ThreadAttributes::allocationCount);}
	if(simpleBufferCount) {printf("simpleBufferCount: %I64d\n", simpleBufferCount);}
	return;
}
#endif
#if 0
void process_exit(const v8::FunctionCallbackInfo<v8::Value>& info) {
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	process_getStats(info);
	getchar();
	if(info.Length() > 0) {
		ExitProcess(info[0]->Uint32Value());
	} else {
		ExitProcess(0);
	}
	return;
}
#endif

#define V8_DEFINE_METHOD
//#include "process_methods.h"
#undef V8_DEFINE_METHOD

#define V8_DEFINE_METHOD_BODIES
//#include "process_methods.h"
#undef V8_DEFINE_METHOD_BODIES

void ums_test() {
	CriticalIsolate iso;
	csIsolate = &iso;
	//#define PersistentSymbol(name) auto symbol_##name = v8::Persistent<v8::String>::New(v8::String::NewFromOneByte(isolate, #name));
	//auto symbol_LowMemoryNotification = v8::Persistent<v8::String>::New(v8::String::NewFromOneByte(isolate, "LowMemoryNotification"));
	//#define DirectFunction(obj, name) PersistentSymbol(name) obj->Set(symbol_##name, v8::FunctionTemplate::New(func_##name));
	//#define DirectMethod(obj, name) PersistentSymbol(name) obj->Set(symbol_##name, v8::FunctionTemplate::New(func_##obj_##name));
	//PersistentSymbol(LowMemoryNotification)
	//process->Set(v8::String::NewFromOneByte(isolate, "LowMemoryNotification"), v8::FunctionTemplate::New(LowMemoryNotification));
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//PersistentSymbol(LowMemoryNotification)
	//iso.allocatedIsolate->Enter();
	//auto isolate = v8::Isolate::New();
	//isolate->Enter();
	//csIsolate->protectedScope([]() {
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
		// Create a new context.

		// Create a template for the global object.
		auto contextObject = v8::ObjectTemplate::New();
		auto global = v8::ObjectTemplate::New();
		auto console = v8::ObjectTemplate::New();
		auto fs = v8::ObjectTemplate::New();
		auto process = v8::ObjectTemplate::New();

		fs->Set(v8::String::NewFromOneByte(isolate, "open"), v8::FunctionTemplate::New(CallOpen::call));
		fs->Set(v8::String::NewFromOneByte(isolate, "write"), v8::FunctionTemplate::New(CallWrite::call));
		fs->Set(v8::String::NewFromOneByte(isolate, "close"), v8::FunctionTemplate::New(CallClose::call));
		fs->Set(v8::String::NewFromOneByte(isolate, "openSync"), v8::FunctionTemplate::New(func_fs_openSync));
		fs->Set(v8::String::NewFromOneByte(isolate, "writeSync"), v8::FunctionTemplate::New(func_fs_writeSync));
		fs->Set(v8::String::NewFromOneByte(isolate, "closeSync"), v8::FunctionTemplate::New(func_fs_closeSync));
		
		//DirectMethod(process, LowMemoryNotification)
		//DirectMethod(process, uptime)
		//process->Set(v8::String::NewFromOneByte(isolate, "LowMemoryNotification"), v8::FunctionTemplate::New(LowMemoryNotification));
		//process->Set(v8::String::NewFromOneByte(isolate, "uptime"), v8::FunctionTemplate::New(uptime));

		#define V8_DEFINE_SYMBOLS
		//#include "process_methods.h"
		#undef V8_DEFINE_SYMBOLS

		//process->Set(v8::String::NewFromOneByte(isolate, "exit"), v8::FunctionTemplate::New(process_exit));
		//process->Set(v8::String::NewFromOneByte(isolate, "getStats"), v8::FunctionTemplate::New(process_getStats));
		//process->Set(v8::String::NewFromOneByte(isolate, "CallWriteFileTotalCounter"), v8::FunctionTemplate::New(CallWriteFileTotalCounter));

		//console->Set(v8::String::NewFromOneByte(isolate, "time"), v8::FunctionTemplate::New(puts));
		//console->Set(v8::String::NewFromOneByte(isolate, "timeEnd"), v8::FunctionTemplate::New(puts));
		console->Set(v8::String::NewFromOneByte(isolate, "log"), v8::FunctionTemplate::New(puts));
		//fs->Set(v8::String::NewFromOneByte(isolate, "readFile"), v8::FunctionTemplate::New(CallReadFile::call));
		//fs->Set(v8::String::NewFromOneByte(isolate, "writeFile"), v8::FunctionTemplate::New(CallWriteFile::call));
		//fs->Set(v8::String::NewFromOneByte(isolate, "unlink"), v8::FunctionTemplate::New(CallUnlink::call));

		//global->Set(v8::String::NewFromOneByte(isolate, "puts"), v8::FunctionTemplate::New(puts));
		contextObject->Set(v8::String::NewFromOneByte(isolate, "console"), console);
		
		global->Set(v8::String::NewFromOneByte(isolate, "process"), process);
		contextObject->Set(v8::String::NewFromOneByte(isolate, "process"), process);
		global->Set(v8::String::NewFromOneByte(isolate, "console"), console);
		global->Set(v8::String::NewFromOneByte(isolate, "fs"), fs);
		//global->Set(v8::String::NewFromOneByte(isolate, "setTimeout"), v8::FunctionTemplate::New(CallSleep::call));
		//global->Set(v8::String::NewFromOneByte(isolate, "setImmediate"), v8::FunctionTemplate::New(CallSetImmediate::call));
		global->Set(v8::String::NewFromOneByte(isolate, "createSimpleBuffer"), v8::FunctionTemplate::New(createSimpleBuffer));
		contextObject->Set(v8::String::NewFromOneByte(isolate, "createSimpleBuffer"), v8::FunctionTemplate::New(createSimpleBuffer));
		//contextObject->Set(v8::String::NewFromOneByte(isolate, "setImmediate"), v8::FunctionTemplate::New(CallSetImmediate::call));

		//global->Set(v8::String::NewFromOneByte(isolate, "global"), global);
		contextObject->Set(v8::String::NewFromOneByte(isolate, "global"), global);


		auto context = v8::Context::New(nullptr, contextObject);
		context->Enter();

		// Here's how you could create a Persistent handle to the context, if needed.
		v8::Persistent<v8::Context> persistent_context = context;
	
		// Enter the created context for compiling and
		// running the hello world script. 
		v8::Context::Scope context_scope(context);

		// Create a string containing the JavaScript source code.
		//MemFile mf(L"sleeptest.js");

		//MemFile mf(L"asyncFiles.js");
		MemFile mf(L"coffeeTest.js");
		auto source = v8::String::New(mf);

		// Compile the source code.
		auto script = v8::Script::Compile(source);
		v8::Persistent<v8::Script> persistent_script(script);

		// Run the script to get the result.
		//auto result = script->Run();

		V8Environment localV8Environment = {/*v8::Isolate::GetCurrent(), */persistent_context, context_scope, persistent_script};
		v8Environment = &localV8Environment;
		//e = new env(le);
	//});

	ums_init();

	csIsolate->protectedScope([]() {
		//e->isolate->Enter();
		v8Environment->persistent_context->Enter();
		v8Environment->persistent_context.Dispose();
	});
	// The persistent handle needs to be eventually disposed.
}

int _tmain(int argc, _TCHAR* argv[]) {
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	mThrowFailure(QueryPerformanceCounter(&startCounter));
	mThrowFailure(QueryPerformanceFrequency(&frequency));
	mThrowFailure(SetProcessAffinityMask(GetCurrentProcess(), 0x01));
	mThrowFailure(SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS));
	//mThrowFailure(SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_HIGHEST));

	#ifdef USE_QUEUE_IOCP
		puts("USE_QUEUE_IOCP");
	#endif

	#ifdef USE_QUEUE_STD
		puts("USE_QUEUE_STD");
	#endif

	#ifdef USE_QUEUE_ATOMIC
		puts("USE_QUEUE_ATOMIC");
	#endif

	#ifdef USE_QUEUE_INTERLOCKED
		puts("USE_QUEUE_INTERLOCKED");
	#endif

	#ifdef USE_QUEUE_STD
		mThrowFailure(startedEvent = CreateEvent(nullptr, true, false, nullptr));
	#endif
	auto tickStart = GetTickCount64();
	ums_test();
	auto tickEnd = GetTickCount64();
	#ifdef USE_QUEUE_STD
		mThrowFailure(CloseHandle(startedEvent));
	#endif
	printf("ticks %I64u\n", tickEnd - tickStart);
	//printf("CallWriteFile::totalCounter %fms\n", static_cast<double>(CallWriteFile::totalCounter * 1000)/static_cast<double>(frequency.QuadPart));

	puts("press any key to exit!\n");
	getchar();
	return 0;
}