#ifndef BLACKLIST_H_INCLUDED
#define BLACKLIST_H_INCLUDED

class Blacklist
{
    private:
    std::vector<std::string> list;

    public:
    BOOL CheckList(char *name);
    BOOL AddToList(char *name, BOOL);
    BOOL RemoveFromList(char *name);
    void OutputList();
};


#endif // BLACKLIST_H_INCLUDED
