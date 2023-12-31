#ifndef LORA_NETWORK_UTIL_H
#define LORA_NETWORK_UTIL_H

#define ENABLE_NET_LOGS

#ifdef ENABLE_NET_LOGS
#include "../../../util/inc/util.h"
#define NET_LOG(format, ...) print("Network", format, ##__VA_ARGS__)
#else
#define NET_LOG(format, ...) do{}while(0)
#endif
#endif
