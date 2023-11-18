#ifndef LORA_STRLIB_H
#define LORA_STRLIB_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

ssize_t strnchr(const char *pSrc, size_t srcLen, char ch);
int strncmp(const char *pA, const char *pB, size_t maxLen);

#ifdef __cplusplus
}
#endif
#endif
