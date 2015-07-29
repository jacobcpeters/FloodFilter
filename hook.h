#ifndef HOOK_H_INCLUDED
#define HOOK_H_INCLUDED

#include "main.h"


HRESULT IATFuncHook(HMODULE Module, const char* ImportedModName, const char* ImportedFuncName, DWORD NewFunc, DWORD Ordinal, PVOID OldFunc);


#endif // HOOK_H_INCLUDED
