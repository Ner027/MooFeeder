#ifndef FEEDINGSTATION_COMMON_H
#define FEEDINGSTATION_COMMON_H


#if defined(_WIN32) || defined(unix) || defined(_WIN64)
#define HOST
#else
#define TARGET
#endif

#endif