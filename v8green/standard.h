#pragma once

//#define MAX_UMS_THREADS 2048
//#define MAX_UMS_THREADS 128
//#define MAX_UMS_THREADS 16384
#define MAX_UMS_THREADS 100
//#define MAX_UMS_THREADS 100

//#define USE_OVERLAPPED
#define UMS_YIELD_RESUME
//#define USE_QUEUE_INTERLOCKED //debug 133              t(7685) tt(1,181,928)
#define USE_QUEUE_IOCP        //debug 142(1000) 139(1) t(8870) tt(3,893,631)
//#define USE_QUEUE_STD         //debug 135              t(7642) tt(3,058,977)
//#define USE_QUEUE_ATOMIC //debug

#if defined(USE_QUEUE_IOCP) + defined(USE_QUEUE_STD) + defined(USE_QUEUE_ATOMIC) + defined(USE_QUEUE_INTERLOCKED) != 1
	#error must define one of USE_QUEUE_IOCP USE_QUEUE_STD USE_QUEUE_ATOMIC USE_QUEUE_INTERLOCKED
#endif

#ifdef USE_QUEUE_ATOMIC
	#error USE_QUEUE_ATOMIC not yet complete
#endif //USE_QUEUE_ATOMIC

#pragma warning(push)
#pragma warning(disable: 4100 4127 4189 4267 4350 4355 4365 4510 4514 4610 4706 4710 4820 4946 4668 4820 4668 4061 4242 4191 4244 4245 4512 4623 4625 4626)
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>

//#pragma warning(push)
#pragma warning(disable: 4100 4127 4189 4267 4350 4355 4365 4510 4514 4610 4706 4710 4820 4946 4668 4820 4668)
#pragma warning(disable: 6011 6297 6326 4800)
#include <v8.h>
//#pragma warning(pop)
#include <Windows.h>
#include "MemFile.h"
#include <typeinfo>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <Strsafe.h>
#pragma warning(pop)
#pragma warning(disable: 4514 4710)

void createSimpleBuffer(void *buf, size_t len);

#define TRUE 1
#define FALSE 0

typedef std::function<void ()> EmptyLambda;

#include "IOCompletionPort.h"
extern IOCompletionPort<EmptyLambda> iocpCall;


#include "CallBase.h"
#include "CallClose.h"
#include "CallOpen.h"
#include "CallPuts.h"
#include "CallReadFile.h"
#include "CallSetImmediate.h"
#include "CallSleep.h"
#include "CallUnlink.h"
#include "CallWrite.h"
#include "CallWriteFile.h"
#include "Critical.h"
#include "CriticalIsolate.h"

extern volatile uint64_t outStandingCallbacks;
extern pCriticalIsolate csIsolate;

#include "slist.h"
typedef slist<PUMS_CONTEXT> slistUmsContext;
typedef slist<PUMS_CONTEXT>::PENTRY pEntryUmsContext;

extern slistUmsContext contextList;
extern slistUmsContext contextSuspendedList;

#include "ThreadAttributes.h"
#include "InterlockedQueue.h"
#include "MemFile.h"
#include "UmsCompletionList.h"
DWORD WINAPI threadProc(__in  LPVOID lpParameter);
#include "UmsThread.h"
#include "IOCPHandle.h"

typedef struct _V8Environment {
	//v8::Isolate *isolate;
	v8::Persistent<v8::Context> persistent_context;
	v8::Context::Scope context_scope;
	v8::Persistent<v8::Script> persistent_script;
} V8Environment, *pV8Environment;
