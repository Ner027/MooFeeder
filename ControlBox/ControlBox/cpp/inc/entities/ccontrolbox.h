#ifndef CONTROLBOX_CCONTROLBOX_H
#define CONTROLBOX_CCONTROLBOX_H

#include <string>
#include "../cloudtypes.h"
#include "../util/cnetworkmanager.h"

enum ControlBoxStatus_et
{
    LOGGED_OUT  = 0x00,
    LOGGED_IN   = 0x01
};

class CControlBox
{
public:
    static CControlBox* getInstance();
    static void killInstance();
    UserReturnCode_et executeLogin(const std::string& username, const std::string& password);
    void executeLogout();
    ControlBoxStatus_et getStatus();
private:
    CControlBox();
    static CControlBox* m_instance;
    std::string m_sessionToken;
    std::string m_name;
    std::string m_username;
    ControlBoxStatus_et m_status;
};


#endif
