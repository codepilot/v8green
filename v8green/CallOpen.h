#pragma once

typedef class CallOpen: public CallBase {
public:
	//#include "ClassHeap.h"
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
//	const static auto stypeNum = CallID::CallOpenID;
	//static volatile LONG64 totalCounter;
	std::wstring WSfilename;
	v8::Persistent<v8::Function> pcb;
	CallOpen(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb);

	void call();

	static void empty();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
	void cbfunc(DWORD bytes, ULONG_PTR key);
	static void static_cbfunc(CallOpen *obj, DWORD bytes, ULONG_PTR key);
	static void static_cbfuncB(CallOpen *obj) {
		obj->call();
	}
} *pCallOpen;
//#include "ClassHeapDef.h"
//ClassHeapDef(CallOpen)
//volatile int64_t CallOpen::totalCounter = 0;

void func_fs_openSync(const v8::FunctionCallbackInfo<v8::Value>& info);
