#include "main.h"

sqlite3 *database;

BOOL openDB()
{
    if(sqlite3_open("FloodFilter.sqlite", &database))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

VOID closeDB()
{
    sqlite3_close(database);
}

VOID initDBTables()
{
    sqlite3_exec(database, "CREATE TABLE IF NOT EXISTS Blacklist (name TEXT PRIMARY KEY)", 0,0,0);
    return;
}
