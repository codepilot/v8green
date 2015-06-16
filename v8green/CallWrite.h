#pragma once

typedef class CallWrite: public CallBase {
public:
	//#include "ClassHeap.h"
	//static volatile LONG64 totalCounter;
	LARGE_INTEGER tickStart;
	LARGE_INTEGER tickEnd;
//	const static auto stypeNum = CallID::CallWriteID;

	uint32_t fd;
	void *buffer;
	uint32_t offset;
	uint32_t length;
	uint32_t position;
	v8::Persistent<v8::Function> callback;
	v8::Persistent<v8::Object> bufferObj;


	void call();

	CallWrite(uint32_t fd, void *buffer, uint32_t offset, uint32_t length, uint32_t position, v8::Persistent<v8::Object> bufferObj, v8::Persistent<v8::Function> callback);
	static void call(const v8::FunctionCallbackInfo<v8::Value>& info);
} *pCallWrite;

void func_fs_writeSync(const v8::FunctionCallbackInfo<v8::Value>& info);
