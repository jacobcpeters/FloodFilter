#ifndef FILTER_H_INCLUDED
#define FILTER_H_INCLUDED

#include "main.h"

struct chatLogEntry
{
    string name, msg;
    UINT score, time;
    bool del;
};

VOID processMsg(ChatMsg*);
VOID processScore();
VOID pruneChatLog();
bool txtFilter(char );
bool msgFilter(chatLogEntry);

#endif // FILTER_H_INCLUDED
