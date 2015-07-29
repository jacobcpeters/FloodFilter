#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

struct ChatMsg
{
    std::string name;
    std::string msg;
};


class ChatStack
{
private:
    std::stack<ChatMsg> queue;

public:
    ChatMsg pop();
    void push(const char *name, const char *msg);
    BOOL empty();
};

#endif // STACK_H_INCLUDED
