#include "standard.h"

CallUnlink::CallUnlink(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb): WSfilename(reinterpret_cast<wchar_t*>(v8::String::Value(filename).operator*())), pcb(v8::Persistent<v8::Function>::New(pcb)) {
	//
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallUnlink::call() {
	mThrowFailure(QueryPerformanceCounter(&tickStart));
	mThrowFailure(DeleteFile(WSfilename.c_str()));
	mThrowFailure(QueryPerformanceCounter(&tickEnd));
//	_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
	csIsolate->protectedScope([this](){
		v8::Handle<v8::Value> args[1] = {v8::Undefined()};
		auto cb = v8::Local<v8::Function>::New(pcb);
		pcb.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

void CallUnlink::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 2) { return; }
	if(!info[0]->IsString()) { return; }
	if(!info[1]->IsFunction()) { return; }
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	//push(new CallUnlink(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[1])));
	auto item = new CallUnlink(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[1]));
	//iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallUnlink*)item)->call(); }, (ULONG_PTR)item);
	return;
}