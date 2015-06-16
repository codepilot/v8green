#include "standard.h"

void CallWrite::call() {
	DWORD bytesWritten = 0;
	#define USE_OVERLAPPED
	#ifdef USE_OVERLAPPED
		OVERLAPPED ov = {0,0,0,0,0};
		mThrowFailure(QueryPerformanceCounter(&tickStart));

		int wfRet;
		mThrowFailureAllow({ERROR_IO_PENDING}, wfRet = WriteFile(reinterpret_cast<HANDLE>(fd), buffer, length, &bytesWritten, &ov));
		if(!wfRet) {
			return;
			//GetOverlappedResult(reinterpret_cast<HANDLE>(fd), &ov, &bytesWritten, true);
		}
		mThrowFailure(QueryPerformanceCounter(&tickEnd));
//		_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
		if(wfRet) {
			csIsolate->protectedScope([this, bytesWritten](){
				auto cb = v8::Local<v8::Function>::New(callback);
				v8::Handle<v8::Value> args[3] = {v8::Undefined(), v8::Uint32::NewFromUnsigned(bytesWritten), v8::Local<v8::Object>::New(bufferObj)};
				bufferObj.Dispose();
				callback.Dispose();
				delete this;
				cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
				_InterlockedDecrement(&outStandingCallbacks);
			});
		}
	#else
		mThrowFailure(QueryPerformanceCounter(&tickStart));
		mThrowFailure(WriteFile(reinterpret_cast<HANDLE>(fd), buffer, length, &bytesWritten, nullptr));
		mThrowFailure(QueryPerformanceCounter(&tickEnd));
		_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);
		csIsolate->protectedScope([this, bytesWritten](){
			auto cb = v8::Local<v8::Function>::New(callback);
			v8::Handle<v8::Value> args[3] = {v8::Undefined(), v8::Uint32::NewFromUnsigned(bytesWritten), v8::Local<v8::Object>::New(bufferObj)};
			bufferObj.Dispose();
			callback.Dispose();
			delete this;
			cb->Call(v8::Context::GetCurrent()->Global(), ARRAYSIZE(args), args);
			_InterlockedDecrement(&outStandingCallbacks);
		});
	#endif
	#undef USE_OVERLAPPED
}

CallWrite::CallWrite(uint32_t fd, void *buffer, uint32_t offset, uint32_t length, uint32_t position, v8::Persistent<v8::Object> bufferObj, v8::Persistent<v8::Function> callback):
	fd(fd), buffer(buffer), offset(offset), length(length), position(position), bufferObj(bufferObj), callback(callback) {
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallWrite::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 6) { return; }
	//if(!info[0]->) { return; }
	//if(!info[0]->IsString()) { return; }
	//if(!info[0]->IsString()) { return; }
	//if(!info[0]->IsString()) { return; }
	//if(!info[5]->IsFunction()) { return; }
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	/*push(new CallWrite(
		info[0]->Uint32Value(),
		info[1]->ToObject()->GetIndexedPropertiesExternalArrayData(),
		info[2]->Uint32Value(),
		info[3]->Uint32Value(),
		info[4]->Uint32Value(),
		v8::Persistent<v8::Object>::New(v8::Local<v8::Object>::Cast(info[1])),
		v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(info[5]))
		));*/
	auto item = new CallWrite(
		info[0]->Uint32Value(),
		info[1]->ToObject()->GetIndexedPropertiesExternalArrayData(),
		info[2]->Uint32Value(),
		info[3]->Uint32Value(),
		info[4]->Uint32Value(),
		v8::Persistent<v8::Object>::New(v8::Local<v8::Object>::Cast(info[1])),
		v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(info[5]))
		);
	iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallWrite*)item)->call(); }, (ULONG_PTR)item);
	return;
}

void func_fs_writeSync(const v8::FunctionCallbackInfo<v8::Value>& info) {
	//(fd, buffer, offset, length, position)
	if(info.Length() < 5) { return; }
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	OVERLAPPED ov = {0,0,0,0,0};
	ov.Pointer = reinterpret_cast<decltype(ov.Pointer)>(info[4]->Uint32Value());
	int wfRet;
	DWORD bytesWritten;
	HANDLE fd = reinterpret_cast<HANDLE>(info[0]->Uint32Value());
	uint8_t *buf = (uint8_t *)info[1]->ToObject()->GetIndexedPropertiesExternalArrayData();
	DWORD bufSize = info[3]->Uint32Value();
	mThrowFailureAllow({ERROR_IO_PENDING}, wfRet = WriteFile(fd, buf, bufSize, &bytesWritten, &ov));
	mThrowFailure(GetOverlappedResult(fd, &ov, &bytesWritten, true));
	return scope.Close(v8::Uint32::NewFromUnsigned(bytesWritten));
}