#ifndef CONTROLBOX_CSTATIONMANAGER_H
#define CONTROLBOX_CSTATIONMANAGER_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <atomic>
#include <map>
#include <QString>
#include "ptwrapper/cthread.h"
#include "LoRa/network/server/cloranetwork.h"
#include "entities/cfeedingstation.h"
#include "station_types.h"

/***********************************************************************************************************************
 * Macros
 ******************************************************************************************************************++**/
#define ENABLE_MANAGER_LOGS

#ifdef ENABLE_MANAGER_LOGS
#include "util/util.h"
#define MANAGER_LOG(format, ...) print("Station Manager", format, ##__VA_ARGS__)
#else
#define MANAGER_LOG(format, ...) do{}while(0)
#endif

class CStationManager : public CThread
{
public:
    /// \brief This method returns a singleton instance of the CStationManager class
    /// \return Pointer to the instance
    static CStationManager* getInstance();

    /// \brief This method allows to retrieve specific details about a Feeding Station based on its physical address
    /// \param phyTag Feeding Station physical address
    /// \param pStationData Pointer to structure where station data will be returned
    /// \return -EINVAL if invalid arguments are passed, -ENODEV if no station with passed physical address is found
    /// returns 0 on success
    int getStationData(QString& phyTag, station_data_st* pStationData);

    /// \brief This method allows to add a new Feeding Station to the Station Manager
    /// \param newStation Details of the station to add to the Station Manager
    /// \return EALREADY if station already existed and its details where overwritten, 0 when a new station is inserted
    int registerStation(station_data_st &newStation);

    /// \brief This method allows to retrieve a list of the Feeding Stations associated with the currently logged in account
    /// \return Vector containing a list of CFeedingStation object
    std::vector<CFeedingStation> getStationList();
private:
    /// \brief Default constructor
    /// \note Made private to only allow instantiation from singleton
    CStationManager();
    static CStationManager* m_instance;
    std::atomic<bool> m_keepRunning;
    CLoRaNetwork* m_networkInstance;
    CMutex m_activeStationsMutex;
    std::map<uint8_t, station_data_st> m_activeStations;
    void* run(void *) override;
};

#endif
