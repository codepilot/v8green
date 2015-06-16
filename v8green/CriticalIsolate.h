#pragma once

typedef class CriticalIsolate {
public:
	//#include "ClassHeap.h"
	v8::Isolate *isolate;
	v8::Isolate *allocatedIsolate;
	Critical crit;
	CriticalIsolate(v8::Isolate *iIsolate);
	CriticalIsolate();
	~CriticalIsolate();
	
	/*
	void protectedScope(std::function<void (v8::HandleScope *)> t) {
		crit.protect([this, t](){
			isolate->Enter();
				v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

			t(&scope);
		});
	}
	*/
	void protectedScope(std::function<void ()> t);
	void protect(std::function<void ()> t);
} *pCriticalIsolate;