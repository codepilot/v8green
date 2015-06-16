#pragma once

typedef class CallSetImmediate: public CallBase {
public:
	//#include "ClassHeap.h"
	v8::Persistent<v8::Function> pcb;
//	const static auto stypeNum = CallID::CallSetImmediateID;
	CallSetImmediate(v8::Persistent<v8::Function> pcb);

	void call();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallSetImmediate;

//#include "ClassHeapDef.h"
//ClassHeapDef(CallSetImmediate)