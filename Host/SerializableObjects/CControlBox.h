#ifndef CB_CCONTROLBOX_H
#define CB_CCONTROLBOX_H
#include "CSerializableObject/CSerializableObject.h"
#include "json.hpp"
using json = nlohmann::json;

class CControlBox : protected CSerializableObject
{
public:
    static CControlBox* getInstance();
    static void killInstance();
    bool loadFromFile(const std::string& filePath);
    bool dumpToFile(const std::string &filePath);
    bool executeLogin(const std::string &username, const std::string &password);
protected:
    bool loadFromJson(std::string&) override;
    std::string dumpToJson() override;
private:
    explicit CControlBox();
    std::string m_sessionToken;
    std::string m_name;
    std::string m_username;
    static CControlBox* m_instance;
};


#endif
