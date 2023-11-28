#ifndef CONTROLBOX_CCONTROLBOX_H
#define CONTROLBOX_CCONTROLBOX_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <string>
#include "../cloudtypes.h"
#include "../util/cnetworkmanager.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
/***********************************************************************************************************************
 * @brief This enum represents the current state of the ControlBox
 **********************************************************************************************************************/
enum ControlBoxStatus_et
{
    LOGGED_OUT  = 0x00,
    LOGGED_IN   = 0x01
};

/***********************************************************************************************************************
 * @brief This class represents a ControlBox instance, it is implemented as a singleton, since only one ControlBox can
 * exist at the same time in this system
 **********************************************************************************************************************/
class CControlBox
{
public:
    /// \brief Get the singleton instance of the ControlBox
    /// \return Pointer to the ControlBox instance
    static CControlBox* getInstance();

    /// \brief Deletes the ControlBox instance
    static void killInstance();

    /// \brief This method tries to perform a login action to the Cloud system using the provided parameters
    /// \param username String containing the username
    /// \param password String containing the password
    /// \return Returns USER_OK if user was logged in with success, INTERNAL_ERROR if unable to perform the web request,
    /// or a corrupted/wrong frame was received, otherwise returns the code received from the Cloud system
    CloudReturnCode_et executeLogin(const std::string& username, const std::string& password);

    /// \brief This method performs a session logout from the ControlBox
    void executeLogout();

    /// \brief This method provides the current status of the ControlBox
    /// \return Returns LOGGED_OUT if this ControlBox is logged out, otherwise returns LOGGED_IN
    ControlBoxStatus_et getStatus();

    std::string getSessionToken();
private:
    /// \brief Constructor made private to ensure this class can not be created from the outside
    CControlBox();

    static CControlBox* m_instance;
    std::string m_sessionToken;
    std::string m_name;
    std::string m_username;
    ControlBoxStatus_et m_status;
};


#endif
