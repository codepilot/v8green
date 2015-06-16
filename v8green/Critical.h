#pragma once

class Critical {
public:
	//#include "ClassHeap.h"
	CRITICAL_SECTION cs;
	Critical();
	_Acquires_lock_(this->cs) void enter();
	_Releases_lock_(this->cs) void leave();
	void protect(std::function<void ()> t);
	~Critical();
};
//#include "ClassHeapDef.h"
//ClassHeapDef(Critical)