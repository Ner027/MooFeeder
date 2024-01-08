#ifndef CONTROLBOX_CCALF_H
#define CONTROLBOX_CCALF_H

#include <string>
#include <QJsonObject>
#include "clog.h"
#include "cserializableobject.h"

class CCalf : public CSerializableObject
{
public:
    explicit CCalf(const std::string& phyTag);
    CCalf(std::string phyTag, float maxConsumption);

    std::vector<CLog> getLatestLogs();
    float getMaxConsumption();
    float getAllowedConsumption();
    bool isValid();

    int loadFromJson(QJsonObject& jsonObject) override;
    QJsonObject dumpToJson() override;

    int addNewToCloud(uint32_t parent);
    int patchInCloud();
private:
    int getCalfFromCloud(QJsonObject* jsonObject);
    std::string m_phyTag;
    float m_maxConsumption;
    float m_currentConsumption;
    bool m_isValid;
};

#endif
