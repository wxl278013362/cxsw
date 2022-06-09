#include "HashFun.h"
#include <string.h>


S32 BernsteinHash(S8 *Key, S32 Prime)
{
    if ( !Key )
        return -1;

    S32 Hash = 0, i, Len = strlen(Key);
    for ( i = 0; i <  Len; i++ )
    {
        Hash = Prime * Hash + Key[i];
    }

    return Hash;
}

S32 AddHash(S8 *Key)
{
    if ( !Key )
        return 0;

    S32 Hash = 0, i, Len = strlen(Key);
    Hash = Len;
    for ( i = 0 ; i < Len ; i++ )
    {
        Hash += Key[i];
    }

    return 0;
}
