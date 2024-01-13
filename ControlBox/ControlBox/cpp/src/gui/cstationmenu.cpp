#include <QJsonArray>
#include "../../inc/gui/cstationmenu.h"
#include "app/cstationmanager.h"

std::vector<CFeedingStation> CStationMenu::getStationList()
{
    return CStationManager::getInstance()->getStationList();
}
