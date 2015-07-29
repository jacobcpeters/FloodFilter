#include "main.h"

UINT LevenshteinDistance(const string *str1, const string *str2)
{
    const size_t len1 = str1->size(), len2 = str2->size();
    vector<UINT> col(len2+1), prevCol(len2);

    for(UINT i = 0; i < prevCol.size(); i++)
    {
        prevCol[i] = i;
    }
    for(UINT i = 0; i < len1; i++)
    {
        col[0] = i+1;
        for(UINT j = 0; j < len2; j++)
        {
            col[j+1] = min(min( 1 + col[j], 1 + prevCol[1 + j]), prevCol[j] + (str1->at(i)==str2->at(j) ? 0 : 1));
        }
        col.swap(prevCol);
    }
    return prevCol[len2];
}
