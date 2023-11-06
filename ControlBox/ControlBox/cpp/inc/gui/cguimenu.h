#ifndef CONTROLBOX_CGUIMENU_H
#define CONTROLBOX_CGUIMENU_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include "../entities/ccontrolbox.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
/***********************************************************************************************************************
 * @brief This enum is used to identify the type of GUI Menu in use
 ******************************************************************************************************************++**/
enum GuiMenuType_et
{
    MAIN_MENU       = 0x00,
    MONITOR_MENU    = 0x01,
    STATION_MENU    = 0x02,
    BOX_MENU        = 0x03,
    SETTINGS_MENU   = 0x04,
    LOGIN_MENU      = 0x05
};

/***********************************************************************************************************************
 * @brief This class represents an abstract GUI Menu
 ******************************************************************************************************************++**/
class CGuiMenu
{
public:
    CGuiMenu();
    virtual ~CGuiMenu() = default;

    /// \brief This method provides a way to acquire the menu type of the current object
    /// \return The return of this method is dependent on how classes that inherit from it implement it
    virtual GuiMenuType_et getType() = 0;

    /// \brief This method allow to differentiate menus that require the user to be authenticated from other
    /// \return True if the current menu requires login, otherwise returns false
    virtual bool requiresPrivilegedAccess() = 0;

    /// \brief This method can be used for going to the previous menu
    void goBack();

protected:
    CControlBox* m_controlBoxInstance;
};

#endif
