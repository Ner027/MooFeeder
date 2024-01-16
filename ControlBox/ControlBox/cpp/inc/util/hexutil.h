#ifndef HEXUTIL_H
#define HEXUTIL_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <string>

/// \brief This function converts a byte array into an hex string representing said byte array
/// \param pSrc Pointer to source byte array
/// \param srcLen Length of source byte array
/// \return String containing the hexadecimal representation of the byte array
std::string arrayToHex( uint8_t* pSrc, size_t srcLen);

#endif
