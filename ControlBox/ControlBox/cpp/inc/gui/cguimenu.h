#ifndef CONTROLBOX_CGUIMENU_H
#define CONTROLBOX_CGUIMENU_H

enum GuiMenuType_et
{
    MAIN_MENU       = 0x00,
    MONITOR_MENU    = 0x01,
    STATION_MENU    = 0x02,
    BOX_MENU        = 0x03,
    SETTINGS_MENU   = 0x04,
    LOGIN_MENU      = 0x05
};

class CGuiMenu
{
public:
    CGuiMenu() = default;
    virtual ~CGuiMenu() = default;
    virtual GuiMenuType_et getType() = 0;
    void goBack();
};


#endif
