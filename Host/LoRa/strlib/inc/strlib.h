#ifndef LORA_STRLIB_H
#define LORA_STRLIB_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
* Strlib APIs
**********************************************************************************************************************/
/// \brief This functions tries to find a character in a string
/// \param pSrc String to search the character in
/// \param srcLen Maximum length to iterate while trying to find the character
/// \param ch Character to find
/// \return -EINVAL if invalid parameters are passed, -ENOMSG if character was not found, otherwise returns the position
/// where the character was found
ssize_t strnchr(const char *pSrc, size_t srcLen, char ch);

/// \brief This functions compares two strings
/// \param pA String A
/// \param pB String B
/// \param maxLen Max length to which the strings should be compared to
/// \return -EINVAL if invalid parameters are passed, -ENOMSG if strings dont match, otherwise returns 0 if A = B
int strncmp(const char *pA, const char *pB, size_t maxLen);

#ifdef __cplusplus
}
#endif
#endif
