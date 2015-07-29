#include "filter.h"

BOOL FILTER = TRUE;
vector<chatLogEntry> ChatLog;

DWORD WINAPI chatFilter(LPVOID lpParam)
{
    ChatMsg msg;

    while(FILTER)
    {
        if(!MsgQueue.empty())
        {
            msg = MsgQueue.pop();
            processMsg(&msg);
        }
        if(!ChatLog.empty())
        {
            processScore();
            pruneChatLog();
        }
        Sleep(100);
    }
    return S_OK;
}

VOID processMsg(ChatMsg *msg)
{
    //Remove characters that aren't necessary
    msg->msg.erase(remove_if(msg->msg.begin(), msg->msg.end(), txtFilter), msg->msg.end());

    for(vector<chatLogEntry>::iterator i = ChatLog.begin(); i!= ChatLog.end(); ++i)
    {
        if(i->name.compare(msg->name) == 0)
        {
            if(msg->msg.size() < (i->msg.size() + 4) && msg->msg.size() > (i->msg.size() - 4))
            {
                if(LevenshteinDistance(&msg->msg, &i->msg) < 3)
                {
                    i->score += 1;
                    i->time = time(NULL)/60;
                    return;
                }
            }
        }
    }

    chatLogEntry newEntry;

    newEntry.name.assign(msg->name);
    newEntry.msg.assign(msg->msg);
    newEntry.score = 1;
    newEntry.time = time(NULL)/60;
    newEntry.del = FALSE;

    ChatLog.push_back(newEntry);
    return;
}

VOID processScore()
{
    for(vector<chatLogEntry>::iterator i = ChatLog.begin(); i!= ChatLog.end(); ++i)
    {
        if(i->score > 2)
        {
            blacklist.AddToList((char*)i->name.c_str(), TRUE);
            i->del = TRUE;
        }
    }
    return;
}

VOID initBlacklist()
{
    int sqliteRetval;
    char *blockReport;
    sqlite3_stmt *statement;
    int blacklistSize = 0;
    DWORD edx = 0;

    sqliteRetval = sqlite3_prepare_v2(database, "SELECT * FROM Blacklist", -1, &statement, 0);

    if(sqliteRetval != SQLITE_OK)
    {
        return;
    }

    while(TRUE)
    {
        sqliteRetval = sqlite3_step(statement);

        if(sqliteRetval == SQLITE_ROW)
        {
            blacklist.AddToList((char*)sqlite3_column_text(statement, 0), FALSE);
            ++blacklistSize;
        }
        else
        {
            asm("movl %0, %%edx;" : :"r"(edx):"edx");

            blockReport = new char[50];
            sprintf(blockReport, "%d Players are being filtered", blacklistSize);

            CTListBoxAppendText((EmptyClass*)InfoBoxPtr, edx, blockReport, 0xFFFF0000, (char)1);
            delete[] blockReport;
            return;
        }
    }
}


VOID pruneChatLog()
{
    ChatLog.erase(remove_if(ChatLog.begin(), ChatLog.end(), msgFilter), ChatLog.end());
}

bool txtFilter(char c)
{
	return c==' ' || c==':' || c==',' || c=='_' ||
           c=='!' || c=='@' || c=='#' || c=='$' ||
           c=='%' || c=='^' || c=='&' || c=='*' ||
           c=='-' || c=='+' || c=='=' || c=='>' ||
           c=='<' || c=='?' || c==',' || c=='.' ||
           c=='~' || c=='(' || c==')' || c=='|' ||
           c=='[' || c==']' || c==';' || c=='{' || c=='}';
}

bool msgFilter(chatLogEntry c)
{
    return c.time < ((time(NULL)/60) - 20) || c.del == TRUE;
}
