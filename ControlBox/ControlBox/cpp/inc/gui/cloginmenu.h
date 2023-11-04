#ifndef CONTROLBOX_CLOGINMENU_H
#define CONTROLBOX_CLOGINMENU_H

#include <QString>
#include "cguimenu.h"
#include "../cloudtypes.h"
#include "../util/cnetworkmanager.h"

class CLoginMenu : public CGuiMenu
{
public:
    CLoginMenu() = default;
    ~CLoginMenu() override = default;
    GuiMenuType_et getType() override {return LOGIN_MENU;}
    bool requiresPrivilegedAccess() override {return false;}
    UserReturnCode_et loginUser(QString& username, QString& password);
    void logoutUser();
    UserReturnCode_et registerUser(QString& username, QString& password);
private:
};

#endif
