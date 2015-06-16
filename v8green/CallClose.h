#pragma once

#include "CallBase.h"

typedef class CallClose: public CallBase {
public:
	//#include "ClassHeap.h"
	//static volatile LONG64 totalCounter;
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
//	const static auto stypeNum = CallID::CallCloseID;

	uint32_t fd;
	v8::Persistent<v8::Function> callback;

	void call();
	CallClose(uint32_t fd, v8::Persistent<v8::Function> callback);
	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);


} *pCallClose;

//#include "ClassHeapDef.h"
//ClassHeapDef(CallClose)
//volatile int64_t CallClose::totalCounter = 0;

void func_fs_closeSync(const v8::FunctionCallbackInfo<v8::Value>& info);
