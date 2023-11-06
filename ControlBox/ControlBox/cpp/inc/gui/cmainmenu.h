#ifndef CONTROLBOX_CMAINMENU_H
#define CONTROLBOX_CMAINMENU_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <iostream>
#include "cguimenu.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
/***********************************************************************************************************************
 * @brief This class represents the Main GUI Menu
 ******************************************************************************************************************++**/
class CMainMenu : public CGuiMenu
{
public:
    CMainMenu() = default;
    ~CMainMenu() override = default;

    /// \brief Override from parent class
    /// \return Returns MAIN_MENU
    GuiMenuType_et getType() override {return MAIN_MENU;}

    /// \brief Override from parent class
    /// \return Returns false
    bool requiresPrivilegedAccess() override {return false;}
};

#endif
