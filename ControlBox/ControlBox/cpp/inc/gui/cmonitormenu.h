#ifndef CONTROLBOX_CMONITORMENU_H
#define CONTROLBOX_CMONITORMENU_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include "cguimenu.h"
#include "../entities/ccalf.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
/***********************************************************************************************************************
 * @brief This class represents the Calf Monitor GUI menu
 ******************************************************************************************************************++**/
class CMonitorMenu : public CGuiMenu
{
public:
    /// \brief Override from parent class
    /// \return MONITOR_MENU
    GuiMenuType_et getType() override {return MONITOR_MENU;}

    /// \brief Override from parent class
    /// \return true
    bool requiresPrivilegedAccess() override {return true;}

    std::vector<CCalf> getCalfList();
private:
};

#endif
