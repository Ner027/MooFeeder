#ifndef CONTROLBOX_CSTATIONMANAGER_H
#define CONTROLBOX_CSTATIONMANAGER_H

#include <atomic>
#include <map>
#include "ptwrapper/cthread.h"
#include "LoRa/network/server/cloranetwork.h"
#include "station_types.h"

#define ENABLE_MANAGER_LOGS

#ifdef ENABLE_MANAGER_LOGS
#include "util/util.h"
#include "entities/cfeedingstation.h"

#define MANAGER_LOG(format, ...) print("Station Manager", format, ##__VA_ARGS__)
#else
#define MANAGER_LOG(format, ...) do{}while(0)
#endif

class CStationManager : public CThread
{
public:
    CStationManager();
    static CStationManager* getInstance();
    int registerStation(station_data_st& newStation);
    std::vector<CFeedingStation> getStationList();
private:
    static CStationManager* m_instance;
    CMutex m_activeStationsMutex;
    std::atomic<bool> m_keepRunning;
    CLoRaNetwork* m_networkInstance;
    std::map<uint8_t, station_data_st> m_activeStations;
    void* run(void *) override;
};

#endif
