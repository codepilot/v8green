#pragma once

typedef class CallSleep: public CallBase {
public:
	//#include "ClassHeap.h"
	v8::Persistent<v8::Function> pcb;
//	const static auto stypeNum = CallID::CallSleepID;
	CallSleep(v8::Persistent<v8::Function> pcb);

	void call();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallSleep;
//#include "ClassHeapDef.h"
//ClassHeapDef(CallSleep)

