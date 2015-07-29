#include "main.h"

BOOL Blacklist::AddToList(char *name, BOOL output)
{
    char *blockReport, *insert;
    DWORD edx = 0;

    if(!name)
    {
        return FALSE;
    }
    if(WaitForSingleObject(blacklistMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return FALSE;
    }
    if(!list.empty())
    {
        for(vector<string>::iterator i = list.begin(); i!= list.end(); ++i)
        {
            if(i->compare(name) == 0)
            {
                asm("movl %0, %%edx;" : :"r"(edx):"edx");

                blockReport = new char[(strlen(name) + 40)];
                sprintf(blockReport, "%s's messages are already blocked", name);
                CTListBoxAppendText((EmptyClass*)InfoBoxPtr, edx, blockReport, 0xFFFF0000, (char)1);
                delete[] blockReport;

                ReleaseMutex(blacklistMutex);
                return FALSE;
            }
        }
    }

    list.push_back(name);

    insert = new char[(strlen(name) + 40)];
    sprintf(insert, "INSERT INTO Blacklist VALUES('%s')", name);
    sqlite3_exec(database, insert, 0,0,0);
    delete[] insert;

    if(output)
    {
        asm("movl %0, %%edx;" : :"r"(edx):"edx");

        blockReport = new char[(strlen(name) + 35)];
        sprintf(blockReport, "%s's messages have been blocked", name);
        CTListBoxAppendText((EmptyClass*)InfoBoxPtr, edx, blockReport, 0xFFFF0000, (char)1);
        delete[] blockReport;
    }

    ReleaseMutex(blacklistMutex);
    return TRUE;
}

BOOL Blacklist::CheckList(char *name)
{
    if(WaitForSingleObject(blacklistMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return FALSE;
    }
    if(name && (list.empty() == FALSE))
    {
        for(vector<string>::iterator i = list.begin(); i!= list.end(); ++i)
        {
            if(i->compare(name) == 0)
            {
                ReleaseMutex(blacklistMutex);
                return TRUE;
            }
        }
    }
    ReleaseMutex(blacklistMutex);
    return FALSE;
}

BOOL Blacklist::RemoveFromList(char *name)
{
    char *blockReport;
    DWORD edx = 0;

    if(WaitForSingleObject(blacklistMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return FALSE;
    }
    if(list.empty())
    {
        ReleaseMutex(blacklistMutex);
        return FALSE;
    }
    for(vector<string>::iterator i = list.begin(); i!= list.end(); ++i)
    {
        if(i->compare(name) == 0)
        {
            list.erase(i);

            asm("movl %0, %%edx;" : :"r"(edx):"edx");

            blockReport = new char[(strlen(name) + 35)];
            sprintf(blockReport, "%s's messages are no longer blocked", name);
            CTListBoxAppendText((EmptyClass*)InfoBoxPtr, edx, blockReport, 0xFFFF0000, (char)1);
            delete[] blockReport;

            ReleaseMutex(blacklistMutex);
            return TRUE;
        }
    }
    ReleaseMutex(blacklistMutex);
    return FALSE;
}


void Blacklist::OutputList()
{
    char *blockedMsg;
    DWORD edx = 0;

    if(WaitForSingleObject(blacklistMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return ;
    }
    if(list.empty())
    {
        ReleaseMutex(blacklistMutex);
        return;
    }
    for(vector<string>::iterator i = list.begin(); i!= list.end(); ++i)
    {
        asm("movl %0, %%edx;" : :"r"(edx):"edx");
        blockedMsg = new char[i->size() + 30];
        sprintf(blockedMsg, "%s's messages have been blocked", (char*)i->c_str());

        CTListBoxAppendText((EmptyClass*)InfoBoxPtr, edx, blockedMsg, 0xFFFF0000, (char)1);
        delete[] blockedMsg;
    }
    ReleaseMutex(blacklistMutex);
    return;
}

