#include "main.h"

BOOL HOOKED;
DWORD ChatBoxPtr = 0;
DWORD InfoBoxPtr = 0;
Blacklist blacklist;

//Thread data
DWORD   dwThreadId;
HANDLE  hThread;

HANDLE chatStackMutex;
HANDLE blacklistMutex;

//Log for catching data
Log *plog;

//Pointers to hooked functions
LP_CTListBoxAppendText CTListBoxAppendText;
LP_CTEditBoxGetText CTEditBoxGetText;
LP_CTEditBoxClearText CTEditBoxClearText;


void DLL_EXPORT FASTCALL AppendTextSpy(EmptyClass *thisptr, DWORD edx, char* Src, DWORD x, char y)
{
    char *chan;
    char *name;
    char *message;
    char *buf;

    if(!InfoBoxPtr && x == 0xFFFFE0E5)
    {
        InfoBoxPtr = (DWORD)thisptr;
        CTListBoxAppendText(thisptr, edx, Src, x, y);
        initBlacklist();
        return;
    }

    if(!ChatBoxPtr && (x == TRADE || x == SHOUT))
    {
        ChatBoxPtr = (DWORD)thisptr;
    }

    if((x == TRADE || x == SHOUT) && ChatBoxPtr == (DWORD)thisptr)
    {
        buf = new char[strlen(Src) + 1];
        strcpy(buf, Src);

        chan = strtok(buf,"[]>");
        name = strtok(NULL,"[]>");
        message = sizeof(char) + strchr(Src , '>');

        if(blacklist.CheckList(name))
        {
            delete[] buf;
            return;
        }

        if(strlen(message) >= 15)
        {
            MsgQueue.push(name, message);
        }
        delete[] buf;
    }
    else if(x == 0xFFFFFFFF && ChatBoxPtr == (DWORD)thisptr)
    {
        buf = new char[strlen(Src) + 1];
        strcpy(buf, Src);
        name = strtok(buf,"[]>");
        message = sizeof(char) + strchr(Src , '>');

        if(!ChrCmpI(*message, '$') && blacklist.CheckList(name))
        {
            delete[] buf;
            return;
        }
        delete[] buf;
    }


    CTListBoxAppendText(thisptr, edx, Src, x, y);

    return;
}

char* DLL_EXPORT FASTCALL GetTextSpy(EmptyClass *thisptr, DWORD edx)
{
    char *retPtr, *buf, *slash, *cmd, *name;
    retPtr = CTEditBoxGetText(thisptr, edx);
    asm("pushal");

    buf = new char[strlen(retPtr)];
    strcpy(buf, retPtr);

    slash = strtok(buf, " ");
    cmd = strtok(NULL, " ");
    name = strtok(NULL, " ");
    if(slash && (!strcmp(slash, "/FF") || !strcmp(slash, "/ff")))
    {
        if(cmd)
        {
            if(!strcmp(cmd, "list"))
            {
                blacklist.OutputList();
            }
            else if(name)
            {
                if(!strcmp(cmd, "add"))
                {
                    blacklist.AddToList(name, TRUE);
                }
                else if(!strcmp(cmd, "del"))
                {
                    blacklist.RemoveFromList(name);
                }
            }
        }
        CTEditBoxClearText(thisptr, edx);
        delete[] buf;
        asm("popal");
        return NULL;
    }
    delete[] buf;
    asm("popal");
    return retPtr;
}

DWORD WINAPI initDLL()
{
        plog = new Log("FloodFilter.log");
        SetFuncHooks();
        openDB();
        initDBTables();

        chatStackMutex = CreateMutex(NULL, FALSE, NULL);
        blacklistMutex = CreateMutex(NULL, FALSE, NULL);

        hThread = CreateThread(NULL, 0, chatFilter, 0, 0, &dwThreadId);
        SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
        return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            //MessageBox(NULL, "AntiChatSpam Loaded", "Plugin", MB_OK);
            initDLL();
            break;
        case DLL_PROCESS_DETACH:
            FILTER = FALSE;
            WaitForSingleObject(hThread, INFINITE);
            closeDB();
            CloseHandle(chatStackMutex);
            delete plog;
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE; // succesful
}
