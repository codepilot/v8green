#pragma once

BOOL CheckSuccess(DWORD *allowedErrors, size_t numAllowed, BOOL success, const LPSTR timeStamp, const LPSTR fileInfo, const int line, const LPSTR func, const LPSTR callStr);
#define mThrowFailureAllow(allowed, call) {DWORD allowedErrors[] = allowed; CheckSuccess(allowedErrors, ARRAYSIZE(allowedErrors), (BOOL)(call), __TIMESTAMP__, __FILE__, __LINE__, __FUNCSIG__, #call); }
#define mThrowFailure(call) CheckSuccess(nullptr, 0, (BOOL)(call), __TIMESTAMP__, __FILE__, __LINE__, __FUNCSIG__, #call);
