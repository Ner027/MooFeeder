#ifndef CONTROLBOX_CFEEDINGSTATION_H
#define CONTROLBOX_CFEEDINGSTATION_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <string>
#include <QJsonObject>
#include "../entities/cserializableobject.h"

class CFeedingStation : public CSerializableObject
{
public:
    /// \brief Default Constructor
    CFeedingStation() = default;

    /// \brief This constructor creates a Feeding Station object, while setting its physical address value
    /// \param phyAddr Hexadecimal value string representing the Feeding Stations physical address
    explicit CFeedingStation(const std::string& phyAddr);

    /// \brief This method allows to load a Feeding Station from a JSON Object
    /// \param jsonObject JSON Object to retrieve Feeding Station data from
    /// \return Number of serialized fields
    int loadFromJson(QJsonObject& jsonObject) override;


    /// \brief This method allows to dump a station object to a JSON Object
    /// \return JSON Object containing this station data in a JSON representation
    QJsonObject dumpToJson() override;

    /// \brief This method allows to define the network address of this Feeding Station
    /// \param addr New network address
    void setNetworkAddr(uint8_t addr);

    /// \brief This method allows to retrieve this Feeding Station physical address in a numeric representation
    /// \return Feeding Station physical address
    [[nodiscard]] uint32_t getPhyAddress() const;

    /// \brief This method allows to add a new Feeding Station to the Cloud System
    /// \param phyAddr Physical address of the Feeding Station to add
    /// \return -EFAULT if unable to communicate with the cloud system, 0 on success
    static int addNewToCloud(const std::string& phyAddr);
private:
    std::string m_hwId;
    std::string m_stationName;
    uint8_t m_networkAddr = 255;
};

#endif
