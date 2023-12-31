#ifndef LORA_MAC_UTIL_H
#define LORA_MAC_UTIL_H

#define ENABLE_MAC_LOGS

#ifdef ENABLE_MAC_LOGS
#include "../../../util/inc/util.h"
#define MAC_LOG(format, ...) print("MAC", format, ##__VA_ARGS__)
#else
#define MAC_LOG(format, ...) do{}while(0)
#endif
#endif
