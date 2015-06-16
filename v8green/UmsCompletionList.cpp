#include "standard.h"

UmsCompletionList::UmsCompletionList(PUMS_SCHEDULER_ENTRY_POINT umsEntryPoint): umsEntryPoint(umsEntryPoint) {
	mThrowFailure(CreateUmsCompletionList(&completionList));
}

UmsCompletionList::~UmsCompletionList() {
	mThrowFailure(DeleteUmsCompletionList(completionList));
}

PUMS_CONTEXT UmsCompletionList::dequeue(DWORD WaitTimeOut) {
	PUMS_CONTEXT umsthreadlist = nullptr;
	mThrowFailure(DequeueUmsCompletionListItems(completionList, WaitTimeOut, &umsthreadlist));
	return umsthreadlist;
}

void UmsCompletionList::transferQueue(DWORD WaitTimeOut) {
	auto nextThread = dequeue(WaitTimeOut);
	while(nextThread) {
		//auto contextEntry = contextFreeList.pop();
		//contextEntry->obj = nextThread;
		//contextList.push(contextEntry);
		contextList.push(nextThread);
		mThrowFailureAllow({ERROR_INSUFFICIENT_BUFFER}, nextThread = GetNextUmsListItem(nextThread));
	};
}

UmsCompletionList::operator PUMS_COMPLETION_LIST() { return completionList; }

void UmsCompletionList::enter() {
	UMS_SCHEDULER_STARTUP_INFO startupInfo = {UMS_VERSION, completionList, umsEntryPoint, 0};
	mThrowFailure(EnterUmsSchedulingMode(&startupInfo));
}

BOOL UmsCompletionList::isUsmThreadSuspended(PUMS_CONTEXT UmsThread){
	BOOLEAN UmsThreadInformation = 0;
	ULONG ReturnLength = 0;
	mThrowFailure(QueryUmsThreadInformation(UmsThread, UmsThreadIsSuspended, &UmsThreadInformation, sizeof(UmsThreadInformation), &ReturnLength));
	return UmsThreadInformation;
}

BOOL UmsCompletionList::isUsmThreadTerminated(PUMS_CONTEXT UmsThread){
	BOOLEAN UmsThreadInformation = 0;
	ULONG ReturnLength = 0;
	mThrowFailure(QueryUmsThreadInformation(UmsThread, UmsThreadIsTerminated, &UmsThreadInformation, sizeof(UmsThreadInformation), &ReturnLength));
	return UmsThreadInformation;
}