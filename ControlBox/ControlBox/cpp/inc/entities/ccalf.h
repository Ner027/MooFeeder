#ifndef CONTROLBOX_CCALF_H
#define CONTROLBOX_CCALF_H

#include <string>
#include <QJsonObject>
#include "clog.h"

class CCalf
{
public:
    explicit CCalf(const std::string phyTag);
    CCalf(const std::string phyTag, float maxConsumption);

    std::vector<CLog> getLatestLogs();
    std::string getPhyTag();
    float getMaxConsumption();
    float getCurrentConsumption();
private:
    int getCalfFromCloud(QJsonObject* jsonObject);
    std::string m_phyTag;
    float m_maxConsumption;
    float m_currentConsumption;
    float m_consumedVolume;
};

#endif
