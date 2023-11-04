#ifndef CONTROLBOX_CMONITORMENU_H
#define CONTROLBOX_CMONITORMENU_H
#include "cguimenu.h"

class CMonitorMenu : public CGuiMenu
{
public:
    bool requiresPrivilegedAccess() override {return true;}
    GuiMenuType_et getType() override {return MONITOR_MENU;}
};


#endif
