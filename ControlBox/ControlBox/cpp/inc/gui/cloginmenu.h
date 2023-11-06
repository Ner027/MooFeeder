#ifndef CONTROLBOX_CLOGINMENU_H
#define CONTROLBOX_CLOGINMENU_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <QString>
#include "cguimenu.h"
#include "../cloudtypes.h"
#include "../util/cnetworkmanager.h"
#include "../entities/ccontrolbox.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
/***********************************************************************************************************************
 * @brief This class represents the Login GUI Menu, it provides a bridge between the GUI and the backend
 ******************************************************************************************************************++**/
class CLoginMenu : public CGuiMenu
{
public:
    CLoginMenu();
    ~CLoginMenu() override = default;

    /// \brief Override from parent class
    /// \return Returns LOGIN_MENU
    GuiMenuType_et getType() override;

    /// \brief Override from parent class
    /// \return Returns false
    bool requiresPrivilegedAccess() override;

    /// \brief This method tries to login to a new ControlBox session
    /// \param username String containing the username
    /// \param password String containing the password
    /// \return Returns USER_OK if user was logged in with success, INTERNAL_ERROR if unable to perform the web request,
    /// or a corrupted/wrong frame was received, otherwise returns the code received from the Cloud system
    UserReturnCode_et loginUser(QString& username, QString& password);

    /// \brief This method performs a session logout from the ControlBox
    void logoutUser();

    /// \brief This method allows to perform the registration of a new Cloud system user
    /// \param username String containing the username
    /// \param password String containing the password
    /// \return
    UserReturnCode_et registerUser(QString& username, QString& password);
};

#endif
