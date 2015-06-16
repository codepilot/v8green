#include "standard.h"

CallOpen::CallOpen(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb): WSfilename(reinterpret_cast<wchar_t*>(v8::String::Value(filename).operator*())), pcb(v8::Persistent<v8::Function>::New(pcb)) {
	//typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
	//PostQueuedCompletionStatus();
//	iocpCall.postEmptyLambda(static_cbfuncB, this);
}

void CallOpen::call() {
	DWORD bytesWritten = 0;
	QueryPerformanceCounter(&tickStart);
	HANDLE handle;
	#define USE_OVERLAPPED
	#ifdef USE_OVERLAPPED
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr));
	#else
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
	#endif
	auto tcb = [this]() {
		puts("tcb");
		DebugBreak();
	};
	//iocpCall.addHandle(reinterpret_cast<HANDLE>(handle), reinterpret_cast<ULONG_PTR>(&tcb));
	#undef USE_OVERLAPPED
	mThrowFailure(QueryPerformanceCounter(&tickEnd));
//	_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
	csIsolate->protectedScope([this, handle](){
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
		v8::Handle<v8::Value> args[2] = {v8::Undefined(isolate), v8::Uint32::NewFromUnsigned(isolate, reinterpret_cast<uint32_t>(handle))};
		auto cb = v8::Local<v8::Function>::New(isolate, pcb);
		pcb.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

void CallOpen::empty() {
}

void CallOpen::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 3) { return; }
	if(!info[0]->IsString()) { return; }
	if(!info[1]->IsString()) { return; }
	if(!info[2]->IsFunction()) { return; }
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
	//push(new CallOpen(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[2])));
	auto item = new CallOpen(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[2]));
	iocpCall.postEmptyLambda([](ULONG_PTR item){
		((CallOpen*)item)->call();
	}, (ULONG_PTR)item);
	//fixme
	return;
}

void CallOpen::cbfunc(DWORD bytes, ULONG_PTR key) {
	printf("CallOpen(%p)::cbfunc(%u, %I64u);\n", this, bytes, key);
	call();
}

void CallOpen::static_cbfunc(CallOpen *obj, DWORD bytes, ULONG_PTR key) {
	printf("CallOpen::static_cbfunc(%p, %u, %I64u);\n", obj, bytes, key);
	obj->cbfunc(bytes, key);
}

void func_fs_openSync(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 2) { return; }
	if(!info[0]->IsString()) { return; }
	if(!info[1]->IsString()) { return; }
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	HANDLE handle;
	mThrowFailure(handle = CreateFile(reinterpret_cast<wchar_t*>(v8::String::Value(v8::Local<v8::String>::Cast(info[0])).operator*()), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr));
	return scope.Close(v8::Uint32::NewFromUnsigned(reinterpret_cast<uint32_t>(handle)));
}