#pragma once

typedef class CallUnlink: public CallBase {
public:
	//#include "ClassHeap.h"
	//static volatile LONG64 totalCounter;
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
	std::wstring WSfilename;
	v8::Persistent<v8::Function> pcb;
//	const static auto stypeNum = CallID::CallUnlinkID;
	CallUnlink(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb);

	void call();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallUnlink;

//#include "ClassHeapDef.h"
//ClassHeapDef(CallUnlink)


//volatile int64_t CallUnlink::totalCounter = 0;