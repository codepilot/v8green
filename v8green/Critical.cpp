#include "standard.h"

Critical::Critical() {
	InitializeCriticalSection(&cs);
}
_Acquires_lock_(this->cs) void Critical::enter() {
	EnterCriticalSection(&cs);
}
_Releases_lock_(this->cs) void Critical::leave() {
	LeaveCriticalSection(&cs);
}
void Critical::protect(std::function<void ()> t) {
	enter();
	t();
	leave();
}
Critical::~Critical() {
	DeleteCriticalSection(&cs);
}
