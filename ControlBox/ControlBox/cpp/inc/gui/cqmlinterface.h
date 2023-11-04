#ifndef CQMLINTERFACE_H
#define CQMLINTERFACE_H

#include <QObject>
#include <map>
#include <stack>
#include "cguimenu.h"

class CQmlInterface : public QObject
{
    Q_OBJECT
public:
    ~CQmlInterface();
    static CQmlInterface* getInstance();
    static void killInstance();
private:
    CQmlInterface();
    std::map<GuiMenuType_et, CGuiMenu*> m_guiMenus;
    std::stack<CGuiMenu*> m_menuStack;
    static CQmlInterface* m_instance;
    void registerMenu(GuiMenuType_et menuType);
public slots:
    void changeToMenu(int newMenu);
    void goBack();
    void shutdownBox();
    void rebootBox();
    int loginUser(QString username, QString password);
    void logoutUser();
    int registerUser(QString username, QString password);
    int getBoxStatus();
signals:
    void menuChanged(int newMenu);
    void menuBack();
    void boxStatusChanged(int newStatus);
};
#endif
