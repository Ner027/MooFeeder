#ifndef CONTROLBOX_CFEEDINGSTATION_H
#define CONTROLBOX_CFEEDINGSTATION_H

#include <string>
#include <QJsonObject>
#include "../entities/cserializableobject.h"

class CFeedingStation : public CSerializableObject
{
public:
    CFeedingStation() = default;
    explicit CFeedingStation(const std::string& phyAddr);
    int loadFromJson(QJsonObject& jsonObject) override;
    QJsonObject dumpToJson() override;
    void setNetworkAddr(uint8_t addr);
    [[nodiscard]] uint32_t getPhyAddress() const;
    static int addNewToCloud(const std::string& phyAddr);
private:
    std::string m_hwId;
    std::string m_stationName;
    uint8_t m_networkAddr = 255;
};

#endif
