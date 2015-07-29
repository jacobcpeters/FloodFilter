#include "hook.h"

#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

DWORD IATFuncHook(HMODULE Module,
                    const char* ImportedModName,
                    const char* ImportedFuncName,
                    DWORD NewFunc,
                    DWORD Ordinal,
                    DWORD *OldFunc)
{
    PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)Module;
    PIMAGE_NT_HEADERS NTHeader;
    PIMAGE_IMPORT_DESCRIPTOR ImportDesc;
    UINT Index;


    NTHeader = MakePtr(PIMAGE_NT_HEADERS, DosHeader, DosHeader->e_lfanew);

    if( IMAGE_NT_SIGNATURE != NTHeader->Signature)
    {
        MessageBox(NULL, "IATFuncHook(): ", "Error", MB_OK);
        return HRESULT_FROM_WIN32(ERROR_BAD_EXE_FORMAT);
    }

    ImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, DosHeader,
                          NTHeader->OptionalHeader.DataDirectory
                          [IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for(Index = 0; ImportDesc[Index].Characteristics != 0; Index++)
    {
        PSTR DllName = MakePtr(PSTR, DosHeader, ImportDesc[Index].Name);

        if(StrCmpI(DllName, ImportedModName) == 0)
        {
            // Found the correct DLL
            PIMAGE_THUNK_DATA Thunk, OrgThunk;

            if(!ImportDesc[Index].FirstThunk || !ImportDesc[Index].OriginalFirstThunk)
            {
                MessageBox(NULL, "IATFuncHook(): E_INVALIDARG", "Error", MB_OK);
                return E_INVALIDARG;
            }

            Thunk = MakePtr(PIMAGE_THUNK_DATA,
                             DosHeader, ImportDesc[Index].FirstThunk);
            OrgThunk = MakePtr(PIMAGE_THUNK_DATA,
                                DosHeader, ImportDesc[Index].OriginalFirstThunk);

            for(; OrgThunk->u1.Function != NULL; OrgThunk++, Thunk++)
            {
                if(OrgThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    if(Ordinal != 0)
                    {
                        if(OrgThunk->u1.Ordinal == Ordinal)
                        {
                            //Desired function found, now let's patch it
                            DWORD junk;
                            MEMORY_BASIC_INFORMATION thunkMemInfo;

                            //make page read/write
                            VirtualQuery(Thunk, &thunkMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
                            if(!VirtualProtect(thunkMemInfo.BaseAddress,
                                                thunkMemInfo.RegionSize,
                                                PAGE_EXECUTE_READWRITE,
                                                &thunkMemInfo.Protect))
                            {
                                MessageBox(NULL, "IATFuncHook(): Check Last Error", "Error", MB_OK);
                                return HRESULT_FROM_WIN32(GetLastError());
                            }

                            //Replace Function Pointers


                                *OldFunc = Thunk->u1.Function;

                            if(NewFunc)//Make sure we don't replace with a null value
                            {
                                Thunk->u1.Function = NewFunc;
                            }

                            //Restore original page protection
                            if(!VirtualProtect(thunkMemInfo.BaseAddress,
                                                thunkMemInfo.RegionSize,
                                                thunkMemInfo.Protect,
                                                &junk))
                            {
                                MessageBox(NULL, "IATFuncHook(): Check Last Error", "Error", MB_OK);
                                return HRESULT_FROM_WIN32(GetLastError());
                            }


                            return S_OK;
                        }

                    }
                    continue;
                }

                PIMAGE_IMPORT_BY_NAME import;
                import = MakePtr(PIMAGE_IMPORT_BY_NAME,
                                  DosHeader, OrgThunk->u1.AddressOfData);

                if(StrStrI((char*)import->Name, ImportedFuncName))
                {
                    //Desired function found, now let's patch it
                    DWORD junk;
                    MEMORY_BASIC_INFORMATION thunkMemInfo;

                    //make page read/write
                    VirtualQuery(Thunk, &thunkMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
                    if(!VirtualProtect(thunkMemInfo.BaseAddress,
                                        thunkMemInfo.RegionSize,
                                        PAGE_EXECUTE_READWRITE,
                                        &thunkMemInfo.Protect))
                    {
                        MessageBox(NULL, "IATFuncHook(): Check Last Error", "Error", MB_OK);
                        return HRESULT_FROM_WIN32(GetLastError());
                    }

                    //Replace Function Pointers


                        *OldFunc = Thunk->u1.Function;

                    if(NewFunc)//Make sure we don't replace with a null value
                    {
                        Thunk->u1.Function = NewFunc;
                    }

                    //Restore original page protection
                    if(!VirtualProtect(thunkMemInfo.BaseAddress,
                                        thunkMemInfo.RegionSize,
                                        thunkMemInfo.Protect,
                                        &junk))
                    {
                        MessageBox(NULL, "IATFuncHook(): Check Last Error", "Error", MB_OK);
                        return HRESULT_FROM_WIN32(GetLastError());
                    }


                    return S_OK;
                }
            }
            //Import not found
            MessageBox(NULL, "IATFuncHook(): ERROR_PROC_NOT_FOUND", "Error", MB_OK);
            return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
        }
    }
    //Dll not found
    MessageBox(NULL, "IATFuncHook(): ERROR_MOD_NOT_FOUND", "Error", MB_OK);
    return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
}

void SetFuncHooks()
{

    IATFuncHook(GetModuleHandle("TRose.exe"),
                "TGameCtrl_r.dll",
                "AppendText@CTListBox",
                (DWORD)AppendTextSpy,
                NULL,
                (DWORD*)&CTListBoxAppendText);

    IATFuncHook(GetModuleHandle("TRose.exe"),
                "TGameCtrl_r.dll",
                "get_text@CTEditBox",
                (DWORD)GetTextSpy,
                NULL,
                (DWORD*)&CTEditBoxGetText);

    IATFuncHook(GetModuleHandle("TRose.exe"),
                "TGameCtrl_r.dll",
                "clear_text@CTEditBox",
                NULL,
                NULL,
                (DWORD*)&CTEditBoxClearText);

    HOOKED = TRUE;
    return;
}
