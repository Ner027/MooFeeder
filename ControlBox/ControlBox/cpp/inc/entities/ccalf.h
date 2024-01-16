#ifndef CONTROLBOX_CCALF_H
#define CONTROLBOX_CCALF_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <string>
#include <QJsonObject>
#include "clog.h"
#include "cserializableobject.h"

class CCalf : public CSerializableObject
{
public:

    /// \brief This constructor allows to load a calf from cloud based on its physical RFID tag
    /// \param phyTag Calf RFID Tag ID
    explicit CCalf(const std::string& phyTag);

    /// \brief This constructor allows to load a calf from cloud based on its physical RFID tag, and set its maximum
    /// consumption at the same time
    /// \param phyTag Calf RFID Tag ID
    /// \param maxConsumption Calf maximum allowed consumption
    CCalf(std::string phyTag, float maxConsumption);

    /// \brief This method allows to retrieve a list of the calves latest consumptions log
    /// \return Vector containing a list of the consumption logs
    std::vector<CLog> getLatestLogs();

    /// \brief This method allows to retrieve the maximum consumption this calf is allowed
    /// \return Float value, in liters, of the calves max consumption volume
    float getMaxConsumption();

    /// \brief This method allows to retrieve how much this calf is still allowed to consume
    /// \return Float value, in liters, of the calves allowed consumption
    float getAllowedConsumption();

    /// \brief This method allows to check if the information of this calf is valid or not
    /// \return True if calf was successfully loaded and can be used, otherwise returns false
    bool isValid();

    /// \brief This method allows to load a Calf from a JSON Object
    /// \param jsonObject JSON Object containing the data to load this calf from
    /// \return -EFAULT if passed object is invalid, otherwise returns the number of serialized fields
    int loadFromJson(QJsonObject& jsonObject) override;

    /// \brief This method allows to dump a calf object to a JSON Object
    /// \return JSON Object containing this calf data in a JSON representation
    QJsonObject dumpToJson() override;

    /// \brief This method allows to add this Calf to the cloud system
    /// \param parent Physical address of the parent Feeding Station
    /// \return -EFAULT if unable to communicate with the cloud system, returns 0 on success
    int addNewToCloud(uint32_t parent);

    /// \brief This method allows to update the calf consumption on the cloud system
    /// \param consumedVolume Float representation, in liters, of the volume the calf consumed
    /// \return -EFAULT if unable to communicate with the cloud system, returns 0 on success
    int reportConsumption(float consumedVolume);
private:
    int getFromCloud(QJsonObject &jsonObject);
    std::string m_phyTag;
    float m_maxConsumption;
    float m_currentConsumption;
    bool m_isValid;
};

#endif
