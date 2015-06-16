#pragma once

typedef class CallReadFile: public CallBase {
public:
	//#include "ClassHeap.h"
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
//	const static auto stypeNum = CallID::CallReadFileID;
	//static volatile LONG64 totalCounter;
	std::wstring WSfilename;
	v8::Persistent<v8::Function> pcb;
	CallReadFile(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb);

	void call();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallReadFile;
//#include "ClassHeapDef.h"
//ClassHeapDef(CallReadFile)
//volatile int64_t CallReadFile::totalCounter = 0;