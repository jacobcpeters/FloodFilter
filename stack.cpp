#include "main.h"

ChatMsg ChatStack::pop()
{
    ChatMsg tmp;

    if(WaitForSingleObject(chatStackMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return tmp;
    }
    tmp = queue.top();
    queue.pop();
    ReleaseMutex(chatStackMutex);
    return tmp;
}

void ChatStack::push(const char *name, const char *msg)
{
    ChatMsg tmp;
    tmp.name.assign(name);
    tmp.msg.assign(msg);

    if(WaitForSingleObject(chatStackMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return;
    }

    queue.push(tmp);
    ReleaseMutex(chatStackMutex);
    return;
}

BOOL ChatStack::empty()
{
    BOOL ret;

    if(WaitForSingleObject(chatStackMutex, INFINITE) != WAIT_OBJECT_0)
    {
        return TRUE;
    }

    ret = queue.empty();
    ReleaseMutex(chatStackMutex);
    return ret;
}

ChatStack MsgQueue;
