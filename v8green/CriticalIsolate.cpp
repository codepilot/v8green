#include "standard.h"

CriticalIsolate::CriticalIsolate(v8::Isolate *iIsolate): allocatedIsolate(iIsolate?nullptr:v8::Isolate::New()), isolate(iIsolate?iIsolate:allocatedIsolate) {
	printf("CriticalIsolate %p\n", isolate);
}
CriticalIsolate::CriticalIsolate(): allocatedIsolate(nullptr), isolate(v8::Isolate::GetCurrent()) {
	printf("CriticalIsolate %p\n", isolate);
}
CriticalIsolate::~CriticalIsolate() {
	if(allocatedIsolate) {
		allocatedIsolate->Dispose();
	}
}
	
void CriticalIsolate::protectedScope(std::function<void ()> t) {
	crit.protect([this, t](){
		isolate->Enter();
			v8::Isolate* isolate = v8::Isolate::GetCurrent(); v8::HandleScope scope(isolate);

		t();
	});
}
void CriticalIsolate::protect(std::function<void ()> t) {
	crit.protect([this, t](){
		isolate->Enter();
		t();
	});
}
