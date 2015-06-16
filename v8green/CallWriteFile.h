#pragma once

typedef class CallWriteFile: public CallBase {
public:
	//#include "ClassHeap.h"
	//static volatile LONG64 totalCounter;
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
	std::wstring WSfilename;
	std::vector<uint8_t> VECdata;
	v8::Persistent<v8::Function> pcb;
	v8::Persistent<v8::Object> Pdata;
	void *pBuf;
	DWORD iBufLen;
	BOOL usePdata;
//	const static auto stypeNum = CallID::CallWriteFileID;

	CallWriteFile(v8::Local<v8::String> filename, v8::Local<v8::String> data, v8::Local<v8::Function> pcb);

	CallWriteFile(v8::Local<v8::String> filename, v8::Persistent<v8::Object> data, v8::Local<v8::Function> pcb);

	void call();

	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallWriteFile;

//#include "ClassHeapDef.h"
//ClassHeapDef(CallWriteFile)


//volatile int64_t CallWriteFile::totalCounter = 0;