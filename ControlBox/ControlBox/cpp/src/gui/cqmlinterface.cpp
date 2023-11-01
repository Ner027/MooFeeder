#include "../../inc/gui/cqmlinterface.h"
#include "../../inc/gui/cmainmenu.h"
#include "../../inc/gui/csettingsmenu.h"
#include "../../inc/gui/cloginmenu.h"
#include <QDebug>

CQmlInterface* CQmlInterface::m_instance = nullptr;

#define CHECK_MENU(x) if (m_menuStack.top()->getType() != (x)) return 1

void CQmlInterface::changeToMenu(int _newMenu)
{
    auto newMenu = static_cast<GuiMenuType_et>(_newMenu);

    //Check if the menu to change to is registered
    if (m_guiMenus.count(newMenu) == 0)
        return;

    //If no menu was set yet, set it to this and do nothing else
    if (m_menuStack.empty())
    {
        m_menuStack.push(m_guiMenus[newMenu]);
        return;
    }

    //If the new menu is the already selected menu, do nothing
    if (m_menuStack.top()->getType() == newMenu)
        return;

    m_menuStack.push(m_guiMenus[newMenu]);

    qDebug() << "Changed to menu " << newMenu;

    emit menuChanged(newMenu);
}

CQmlInterface::CQmlInterface()
{
    registerMenu(MAIN_MENU);
    registerMenu(SETTINGS_MENU);
    registerMenu(LOGIN_MENU);
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
            break;
        case STATION_MENU:
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

    return (int) loginMenu->loginUser(username, password);
}

int CQmlInterface::registerUser(QString username, QString password)
{
    CHECK_MENU(LOGIN_MENU);

    auto* loginMenu = dynamic_cast<CLoginMenu*>(m_menuStack.top());

    return (int) loginMenu->loginUser(username, password);
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


