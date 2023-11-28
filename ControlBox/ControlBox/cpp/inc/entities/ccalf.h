#ifndef CONTROLBOX_CCALF_H
#define CONTROLBOX_CCALF_H

#include <string>
#include <QJsonObject>
#include "clog.h"
#include "cserializableobject.h"

class CCalf : public CSerializableObject
{
public:
    explicit CCalf(const std::string phyTag);
    CCalf(const std::string phyTag, float maxConsumption);

    std::vector<CLog> getLatestLogs();
    std::string getPhyTag();
    float getMaxConsumption();
    float getCurrentConsumption();

    int loadFromJson(QJsonObject& jsonObject) override { return 0;}
    QJsonObject dumpToJson() override;
private:
    int getCalfFromCloud(QJsonObject* jsonObject);
    std::string m_phyTag;
    float m_maxConsumption;
    float m_currentConsumption;
};

#endif
