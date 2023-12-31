#ifndef FEEDINGSTATION_NETWORK_CLIENT_H
#define FEEDINGSTATION_NETWORK_CLIENT_H

#include "../../common/inc/network_types.h"

int network_init();
int network_send(network_frame_st* pNetFrame, duration_t maxWait);
int network_receive(network_frame_st* pNetFrame, duration_t maxWait);

#endif
