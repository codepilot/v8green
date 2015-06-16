#include "standard.h"

CallReadFile::CallReadFile(v8::Local<v8::String> filename, v8::Local<v8::Function> pcb): WSfilename(reinterpret_cast<wchar_t*>(v8::String::Value(filename).operator*())), pcb(v8::Persistent<v8::Function>::New(pcb)) {
	//typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallReadFile::call() {
	DWORD bytesWritten = 0;
	mThrowFailure(QueryPerformanceCounter(&tickStart));
	HANDLE handle;
	#ifdef USE_OVERLAPPED
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr));
		OVERLAPPED ov = {0,0,0,0,0};
		BOOL wfRet;
		mThrowFailure(wfRet = ReadFile(handle, VECdata.data(), (DWORD)VECdata.size(), &bytesWritten, &ov));
		mThrowFailure(GetOverlappedResult(handle, &ov, &bytesWritten, true));
		mThrowFailure(CloseHandle(handle));
	#else
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
		BY_HANDLE_FILE_INFORMATION info;
		mThrowFailure(GetFileInformationByHandle(handle, &info));
		ULARGE_INTEGER fileLen;
		fileLen.LowPart = info.nFileSizeLow;
		fileLen.HighPart = info.nFileSizeHigh;
		LPVOID buf;
		mThrowFailure(buf = VirtualAlloc(nullptr, fileLen.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
		mThrowFailure(ReadFile(handle, buf, (DWORD)fileLen.QuadPart, &bytesWritten, nullptr));
		mThrowFailure(CloseHandle(handle));
	#endif
	mThrowFailure(QueryPerformanceCounter(&tickEnd));
//	_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
	csIsolate->protectedScope([this, buf, fileLen](){
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
		v8::Handle<v8::Value> args[2] = {v8::Undefined(isolate), createSimpleBuffer(buf, fileLen.QuadPart)};
		auto cb = v8::Local<v8::Function>::New(isolate, pcb);
		pcb.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

void CallReadFile::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 2) { return; }
	if(!info[0]->IsString()) { return; }
	if(!info[1]->IsFunction()) { return; }
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	//push(new CallReadFile(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[1])));
	auto item = new CallReadFile(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::Function>::Cast(info[1]));
	//iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallReadFile*)item)->call(); }, (ULONG_PTR)item);
	return;
}