#include "../../inc/gui/cqmlinterface.h"
#include "../../inc/gui/cmainmenu.h"
#include "../../inc/gui/csettingsmenu.h"
#include "../../inc/gui/cloginmenu.h"
#include "../../inc/gui/cmonitormenu.h"
#include "../../inc/gui/cstationmenu.h"
#include "app/cstationmanager.h"
#include <QJsonArray>

CQmlInterface* CQmlInterface::m_instance = nullptr;

#define CHECK_MENU(x) if (m_menuStack.top()->getType() != (x)) return 1

void CQmlInterface::changeToMenu(int _newMenu)
{
    auto newMenu = static_cast<GuiMenuType_et>(_newMenu);

    //Check if the menu to change to is registered
    if (m_guiMenus.count(newMenu) == 0)
        return;

    auto menu = m_guiMenus[newMenu];

    //Check if this is a menu which requires the user to be logged in
    if (menu->requiresPrivilegedAccess() && (CControlBox::getInstance()->getStatus() != LOGGED_IN))
        return;

    //If no menu was set yet, set it to this and do nothing else
    if (m_menuStack.empty())
    {
        m_menuStack.push(menu);
        return;
    }

    //If the new menu is the already selected menu, do nothing
    if (m_menuStack.top()->getType() == newMenu)
        return;

    m_menuStack.push(menu);

    qDebug() << "Changed to menu " << newMenu;

    onMenuLoaded(newMenu);

    emit menuChanged(newMenu);
}

CQmlInterface::CQmlInterface()
{
    registerMenu(MAIN_MENU);
    registerMenu(SETTINGS_MENU);
    registerMenu(LOGIN_MENU);
    registerMenu(MONITOR_MENU);
    registerMenu(STATION_MENU);
    changeToMenu(MAIN_MENU);
}

CQmlInterface::~CQmlInterface()
{
    //Delete dynamically allocated objects
    for (auto& item : m_guiMenus)
        delete item.second;
}

void CQmlInterface::registerMenu(GuiMenuType_et menuType)
{
    CGuiMenu* newMenu;

    //Check if this type of menu wasn't registered already
    if (m_guiMenus.count(menuType) != 0)
        return;

    switch (menuType)
    {
        case MAIN_MENU:
            newMenu = new CMainMenu;
            break;
        case MONITOR_MENU:
            newMenu = new CMonitorMenu;
            break;
        case STATION_MENU:
            newMenu = new CStationMenu;
            break;
        case BOX_MENU:
            break;
        case SETTINGS_MENU:
            newMenu = new CSettingsMenu;
            break;
        case LOGIN_MENU:
            newMenu = new CLoginMenu;
            break;
    }

    m_guiMenus[menuType] = newMenu;
}

void CQmlInterface::goBack()
{
    //If the stack depth is less than 2 it means we are at the root menu and can't go back
    if (m_menuStack.size() < 2)
        return;

    m_menuStack.pop();

    qDebug() << "Back to menu " << m_menuStack.top()->getType();

    emit menuBack();
}

CQmlInterface *CQmlInterface::getInstance()
{
    if (!m_instance)
        m_instance = new CQmlInterface;

    return m_instance;
}

void CQmlInterface::killInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

int CQmlInterface::loginUser(QString username, QString password)
{
    CHECK_MENU(LOGIN_MENU);

    auto* loginMenu = dynamic_cast<CLoginMenu*>(m_menuStack.top());

    auto ret = (int) loginMenu->loginUser(username, password);

    emit boxStatusChanged((int)CControlBox::getInstance()->getStatus());

    return ret;
}

int CQmlInterface::registerUser(QString username, QString password)
{
    CHECK_MENU(LOGIN_MENU);

    auto* loginMenu = dynamic_cast<CLoginMenu*>(m_menuStack.top());

    return (int) loginMenu->registerUser(username, password);
}

void CQmlInterface::shutdownBox()
{
    if (m_menuStack.top()->getType() != SETTINGS_MENU)
        return;

    auto* settingsMenu = dynamic_cast<CSettingsMenu*>(m_menuStack.top());

    settingsMenu->platformShutdown();
}

void CQmlInterface::rebootBox()
{
    if (m_menuStack.top()->getType() != SETTINGS_MENU)
        return;

    auto* settingsMenu = dynamic_cast<CSettingsMenu*>(m_menuStack.top());

    settingsMenu->platformReboot();
}

int CQmlInterface::getBoxStatus()
{
    auto ret = (int) CControlBox::getInstance()->getStatus();

    emit boxStatusChanged(ret);

    return ret;
}

void CQmlInterface::logoutUser()
{
    if (m_menuStack.top()->getType() != LOGIN_MENU)
        return;

    auto* loginMenu = dynamic_cast<CLoginMenu*>(m_menuStack.top());

    loginMenu->logoutUser();

    emit boxStatusChanged((int)CControlBox::getInstance()->getStatus());
}

void CQmlInterface::onMenuLoaded(GuiMenuType_et newMenu)
{
    switch (newMenu)
        {
            case MONITOR_MENU:
            {
                QJsonArray jArray;
                auto monitorMenu = dynamic_cast<CMonitorMenu*>(m_menuStack.top());

                emit clearCalfList();
                emit clearCalfSelection();
                emit clearGraph();

                auto calfList = monitorMenu->getCalfList();
                for (auto& item : calfList)
                    jArray.append(item.dumpToJson());

                emit setCalfList(jArray);
                break;
            }

            case STATION_MENU:
            {
                QJsonArray jArray;
                auto stationMenu = dynamic_cast<CStationMenu*>(m_menuStack.top());

                emit clearStationList();
                emit clearStationSelection();

                auto stationList = stationMenu->getStationList();
                for (auto& item : stationList)
                    jArray.append(item.dumpToJson());

                emit setStationList(jArray);
                break;
            }
            default:
                break;
        }
}

void CQmlInterface::selectCalf(QString phyTag)
{
    CCalf calf(phyTag.toStdString());
    auto logs = calf.getLatestLogs();

    emit clearCalfSelection();
    emit clearGraph();
    emit calfSelected(calf.getMaxConsumption(), "Not implemented yet!");

    if (logs.empty())
        return;

    for (auto& item: logs)
        emit addPointToGraph(item.getTimestamp(), item.getConsumedVolume());
}

void CQmlInterface::updateCalfList()
{
    onMenuLoaded(MONITOR_MENU);
}

void CQmlInterface::updateStationList()
{
    onMenuLoaded(STATION_MENU);
}

void CQmlInterface::selectStation(QString hwId, QString name)
{
    int ret;
    station_data_st stationData {0};

    ret = CStationManager::getInstance()->getStationData(hwId, &stationData);

    QString str = QString("FAFEDEAD");

    emit stationSelected(stationData.currTemperature,
                         stationData.currBattery,
                         stationData.netAddr,
                         name,
                         str, (ret >= 0) ? "Connected" : "Not Connected");
}

