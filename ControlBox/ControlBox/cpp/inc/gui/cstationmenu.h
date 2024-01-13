#ifndef CONTROLBOX_CSTATIONMENU_H
#define CONTROLBOX_CSTATIONMENU_H

#include "cguimenu.h"
#include "entities/cfeedingstation.h"

class CStationMenu : public CGuiMenu
{
public:
    GuiMenuType_et getType() override {return STATION_MENU;};
    bool requiresPrivilegedAccess() override {return true;};
    std::vector<CFeedingStation> getStationList();
};


#endif
