#include "standard.h"

CallBase::CallBase() {
	cb = nullptr;
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.Pointer = nullptr;
	overlapped.hEvent = nullptr;
}

CallBase::operator LPOVERLAPPED() {
	return reinterpret_cast<LPOVERLAPPED>(this);
}
