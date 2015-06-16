#include "standard.h"

void CallClose::call() {
	DWORD bytesWritten = 0;
	mThrowFailure(QueryPerformanceCounter(&tickStart));
	mThrowFailure(CloseHandle(reinterpret_cast<HANDLE>(fd)));
	mThrowFailure(QueryPerformanceCounter(&tickEnd));
//	_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
	csIsolate->protectedScope([this, bytesWritten](){
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
		auto cb = v8::Local<v8::Function>::New(isolate, callback);
		v8::Handle<v8::Value> args[1] = {v8::Undefined(isolate)};
		callback.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

CallClose::CallClose(uint32_t fd, v8::Persistent<v8::Function> callback):
	fd(fd), callback(callback) {
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallClose::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
	if(info.Length() < 2) { return; }
	if(!info[0]->IsUint32()) { return; }
	if(!info[1]->IsFunction()) { return; }
	/*push(new CallClose(
		info[0]->Uint32Value(),
		v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(info[1]))
		));*/
	auto item = new CallClose(info[0]->Uint32Value(), v8::Persistent<v8::Function>::New(isolate, v8::Local<v8::Function>::Cast(info[1])));
	iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallClose*)item)->call(); }, (ULONG_PTR)item);
	return;
}

void func_fs_closeSync(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 1) { return; }
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
	HANDLE fd = reinterpret_cast<HANDLE>(info[0]->Uint32Value());
	mThrowFailure(CloseHandle(fd));
	return;
}