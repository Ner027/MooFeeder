#ifndef CONTROLBOX_CLOGINMENU_H
#define CONTROLBOX_CLOGINMENU_H

#include <QString>
#include "cguimenu.h"
#include "../cloudinfo.h"

class CLoginMenu : public CGuiMenu
{
public:
    GuiMenuType_et getType() override {return LOGIN_MENU;}
    LoginReturnCode_et loginUser(QString& username, QString& password);
};

#endif
