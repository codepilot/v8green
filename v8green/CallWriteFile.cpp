#include "standard.h"

CallWriteFile::CallWriteFile(v8::Local<v8::String> filename, v8::Local<v8::String> data, v8::Local<v8::Function> pcb): WSfilename(reinterpret_cast<wchar_t*>(v8::String::Value(filename).operator*())), VECdata(data->Utf8Length()), pcb(v8::Persistent<v8::Function>::New(pcb)), usePdata(false) {
	CopyMemory(VECdata.data(), v8::String::Utf8Value(data).operator*(), VECdata.size());
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

CallWriteFile::CallWriteFile(v8::Local<v8::String> filename, v8::Persistent<v8::Object> data, v8::Local<v8::Function> pcb): WSfilename(reinterpret_cast<wchar_t*>(v8::String::Value(filename).operator*())), Pdata(data), pcb(v8::Persistent<v8::Function>::New(pcb)), usePdata(true), pBuf(Pdata->GetIndexedPropertiesExternalArrayData()), iBufLen(Pdata->GetIndexedPropertiesExternalArrayDataLength()) {
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallWriteFile::call() {
	DWORD bytesWritten = 0;
	mThrowFailure(QueryPerformanceCounter(&tickStart));
	HANDLE handle;
	#ifdef USE_OVERLAPPED
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr));
		OVERLAPPED ov = {0,0,0,0,0};
		BOOL wfRet
		mThrowFailure(wfRef = WriteFile(handle, VECdata.data(), (DWORD)VECdata.size(), &bytesWritten, &ov));
		mThrowFailure(GetOverlappedResult(handle, &ov, &bytesWritten, true));
		mThrowFailure(CloseHandle(handle));
	#else
		mThrowFailure(handle = CreateFile(WSfilename.c_str(), GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
		if(usePdata) {
			mThrowFailure(WriteFile(handle, pBuf, iBufLen, &bytesWritten, nullptr));
		} else {
			mThrowFailure(WriteFile(handle, VECdata.data(), (DWORD)VECdata.size(), &bytesWritten, nullptr));
		}
		mThrowFailure(CloseHandle(handle));
	#endif
	mThrowFailure(QueryPerformanceCounter(&tickEnd));
//	_InterlockedAdd64(&totalCounter, tickEnd.QuadPart - tickStart.QuadPart);

	csIsolate->protectedScope([this](){
		if(usePdata) { Pdata.Dispose(); }
		auto cb = v8::Local<v8::Function>::New(pcb);
		pcb.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), 0, nullptr);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

void CallWriteFile::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 3) { return; }
	if(!info[0]->IsString()) { return; }
	if(!info[2]->IsFunction()) { return; }
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

	if(info[1]->IsString()) {
		//push(new CallWriteFile(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::String>::Cast(info[1]), v8::Local<v8::Function>::Cast(info[2])));
		auto item = new CallWriteFile(v8::Local<v8::String>::Cast(info[0]), v8::Local<v8::String>::Cast(info[1]), v8::Local<v8::Function>::Cast(info[2]));
		//iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallWriteFile*)item)->call(); }, (ULONG_PTR)item);
	} else if(info[1]->IsObject()) {
		auto dataObj = v8::Local<v8::Object>::Cast(info[1]);
		if(!dataObj->HasIndexedPropertiesInExternalArrayData()) { return; }
		if(dataObj->GetIndexedPropertiesExternalArrayData()) {
			auto pBufObj = v8::Persistent<v8::Object>::New(dataObj); 
			//push(new CallWriteFile(v8::Local<v8::String>::Cast(info[0]), pBufObj, v8::Local<v8::Function>::Cast(info[2])));
			auto item = new CallWriteFile(v8::Local<v8::String>::Cast(info[0]), pBufObj, v8::Local<v8::Function>::Cast(info[2]));
			//iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallWriteFile*)item)->call(); }, (ULONG_PTR)item);
		}
	}
	return;
}