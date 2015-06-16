#include "standard.h"

CallSetImmediate::CallSetImmediate(v8::Persistent<v8::Function> pcb): pcb(pcb) {
//		typeNum = stypeNum;
	_InterlockedIncrement(&outStandingCallbacks);
}

void CallSetImmediate::call() {
	csIsolate->protectedScope([this](){
		v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
		auto cb = v8::Local<v8::Function>::New(isolate, pcb);
		pcb.Dispose();
		delete this;
		cb->Call(v8::Context::GetCurrent()->Global(), 0, nullptr);
		_InterlockedDecrement(&outStandingCallbacks);
	});
}

void CallSetImmediate::call(const v8::FunctionCallbackInfo<v8::Value>& info) {
	if(info.Length() < 1) {
		return;
	}
	if(!info[0]->IsFunction()) {
		return;
	}
	v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);
	//push(new CallSetImmediate(v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(info[0]))));
	auto item = new CallSetImmediate(v8::Persistent<v8::Function>::New(isolate, v8::Local<v8::Function>::Cast(info[0])));
	//iocpCall.postEmptyLambda([](ULONG_PTR item){ ((CallSetImmediate*)item)->call(); }, (ULONG_PTR)item);
	return;
}