#ifndef CONTROLBOX_CCONTROLBOX_H
#define CONTROLBOX_CCONTROLBOX_H


#include <string>
#include "../cloudinfo.h"
#include "../util/cnetworkmanager.h"

class CControlBox
{
public:
    static CControlBox* getInstance();
    static void killInstance();
    LoginReturnCode_et executeLogin(const std::string& username, const std::string& password);
private:
    CControlBox();
    static CControlBox* m_instance;
    CNetworkManager* m_networkManager;
    std::string m_sessionToken;
    std::string m_name;
    std::string m_username;
};


#endif
