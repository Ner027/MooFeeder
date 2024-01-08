#ifndef CONTROLBOX_CSTATIONMANAGER_H
#define CONTROLBOX_CSTATIONMANAGER_H

#include <atomic>
#include <map>
#include "ptwrapper/cthread.h"
#include "LoRa/network/server/cloranetwork.h"
#include "station_types.h"

class CStationManager : public CThread
{
public:
    CStationManager();
    static CStationManager* getInstance();
private:
    static CStationManager* m_instance;
    std::atomic<bool> m_keepRunning;
    CLoRaNetwork* m_networkInstance;
    std::map<uint8_t, station_data_st> m_activeStations;
    void* run(void *) override;
};

#endif
