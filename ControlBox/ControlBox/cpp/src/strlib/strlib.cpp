/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "strlib/strlib.h"
#include <cerrno>

/***********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/
ssize_t strnchr(const char* pSrc, const size_t srcLen, char ch)
{
    size_t i;

    if (!pSrc)
        return -EINVAL;

    for (i = 0; (pSrc[i] != '\0') && (i < srcLen); ++i)
    {
        if(pSrc[i] == ch)
            return (ssize_t) i;
    }

    return -ENOMSG;
}

int strncmp(const char* pA, const char* pB, size_t maxLen)
{
    if (!pA || !pB)
        return -EINVAL;

    while (maxLen--)
    {
        if (*pA != *pB)
            return -ENOMSG;
    }

    return 0;
}