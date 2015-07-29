#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <locale>
#include <shlwapi.h>
#include <sqlite3.h>


#include "Blacklist.h"
#include "stack.h"

#define DLL_EXPORT __declspec(dllexport)
#define THISCALL __thiscall
#define FASTCALL __fastcall
#define STDCALL __stdcall

#define TRADE 0xFF00D232
#define SHOUT 0xFFBDFAFF

#define GETWORD(p,o) *((WORD*)&p.Buffer[o])
#define GETDWORD(p,o) *((DWORD*)&p.Buffer[o])
#define GETQWORD(p,o) *((QWORD*)&p.Buffer[o])
#define GETFLOAT(p,o) *((float*)&p.Buffer[o])


using namespace std;

//just an empty class
class EmptyClass {};


//Extern Global Variables
//main.cpp
extern BOOL HOOKED;
extern DWORD ChatBoxPtr;
extern DWORD InfoBoxPtr;
extern Blacklist blacklist;
extern HANDLE chatStackMutex;
extern HANDLE blacklistMutex;
//filter.cpp
extern BOOL FILTER;
//database.cpp
extern sqlite3 *database;
//stack.cpp
extern ChatStack MsgQueue;
//end global variables


//Log Class implemented in log.cpp
class Log {
  public:
    Log(const char* filename);
    ~Log();
    void Write(char* logline);
  private:
    ofstream m_stream;
};

//dll exports
EXTERN_C BOOL DLL_EXPORT WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
EXTERN_C void DLL_EXPORT FASTCALL AppendTextSpy(EmptyClass *thisptr, DWORD edx, char* Src, DWORD x, char y);
EXTERN_C char* DLL_EXPORT FASTCALL GetTextSpy(EmptyClass *thisptr, DWORD edx);

typedef void (FASTCALL* LP_CTListBoxAppendText)(EmptyClass*, DWORD, char*, DWORD, char);
typedef char* (FASTCALL* LP_CTEditBoxGetText)(EmptyClass*, DWORD);
typedef void (FASTCALL* LP_CTEditBoxClearText)(EmptyClass*, DWORD);

extern LP_CTListBoxAppendText CTListBoxAppendText;
extern LP_CTEditBoxGetText CTEditBoxGetText;
extern LP_CTEditBoxClearText CTEditBoxClearText;

DWORD WINAPI initDLL( LPVOID lpParam );

// Extern functions
//filter.cpp
extern DWORD WINAPI chatFilter( LPVOID lpParam );
extern VOID initBlacklist();
//database.cpp
extern BOOL openDB();
extern VOID closeDB();
extern VOID initDBTables();
//IATHook.cpp
extern void SetFuncHooks();
//levDist.cpp
extern UINT LevenshteinDistance(const std::string *str1, const std::string *str2);
//End Extern Functions

#endif // __MAIN_H__
