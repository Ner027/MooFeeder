#ifndef CONTROLBOX_CSETTINGSMENU_H
#define CONTROLBOX_CSETTINGSMENU_H

#include "cguimenu.h"

class CSettingsMenu : public CGuiMenu
{
public:
    GuiMenuType_et getType() override  {return SETTINGS_MENU;}
    bool requiresPrivilegedAccess() override {return false;}
    void platformShutdown();
    void platformReboot();
};


#endif