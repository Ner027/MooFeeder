#include <sstream>
#include <iomanip>
#include "util/hexutil.h"

std::string arrayToHex( uint8_t* pSrc, size_t srcLen)
{
    int num;
    std::stringstream ss;

    while (srcLen--)
    {
        num = (int) pSrc[srcLen];
        ss << std::setfill('0') << std::setw(2) << std::hex << num;
    }

    return ss.str();
}