#ifndef CONTROLBOX_CMAINMENU_H
#define CONTROLBOX_CMAINMENU_H


#include <iostream>
#include "cguimenu.h"

class CMainMenu : public CGuiMenu
{
public:
    CMainMenu() = default;
    ~CMainMenu() override {std::cout << "Deleted Main Menu" << std::endl;}
    GuiMenuType_et getType() override {return MAIN_MENU;}
    bool requiresPrivilegedAccess() override {return false;}
};


#endif
