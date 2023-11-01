#include <cstdlib>
#include "../../inc/gui/csettingsmenu.h"

void CSettingsMenu::platformShutdown()
{
    system("halt");
}

void CSettingsMenu::platformReboot()
{
    system("reboot");
}
