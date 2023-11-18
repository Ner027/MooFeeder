#ifndef CQMLINTERFACE_H
#define CQMLINTERFACE_H

/***********************************************************************************************************************
 * Includes
 ******************************************************************************************************************++**/
#include <QObject>
#include <map>
#include <stack>
#include "cguimenu.h"

/***********************************************************************************************************************
 * Typedefs
 ******************************************************************************************************************++**/
class CQmlInterface : public QObject
{
    Q_OBJECT
public:
    ~CQmlInterface() override;

    /// \brief This method allows to acquire the singleton instance of the QmlInterface class
    /// \return Pointer to the singleton instance
    static CQmlInterface* getInstance();

    /// \brief This method destroys the singleton instance of the QmlInterface class
    static void killInstance();
private:
    /// \brief Constructor made private to ensure this class can not be created from the outside
    CQmlInterface();

    /// \brief This method allows to register a new GUI menu
    /// \param menuType The type of menu to register
    void registerMenu(GuiMenuType_et menuType);

    void onMenuLoaded(GuiMenuType_et menuTypeEt);

    static CQmlInterface* m_instance;
    std::map<GuiMenuType_et, CGuiMenu*> m_guiMenus;
    std::stack<CGuiMenu*> m_menuStack;
public slots:
    /// \brief This slot can be accessed from QML in order to change to a different menu
    /// \param newMenu The menu the GUI should change to
    void changeToMenu(int newMenu);

    /// \brief This slot can be accessed from QML to go back one menu
    void goBack();

    /// \brief This slot allows to shutdown the ControlBox
    void shutdownBox();

    /// \brief This slot allows to reboot the ControlBox
    void rebootBox();

    /// \brief This slot allows QML to start a login process
    /// \param username String containing the username
    /// \param password String containing the password
    /// \return Returns a cast to an integer from a UserReturnCode_et
    int loginUser(QString username, QString password);

    /// \brief This slot allows QML to perform a logout
    void logoutUser();

    /// \brief This slot allows QML to start a registration process
    /// \param username String containing the username
    /// \param password String containing the password
    /// \return Returns a cast to an integer from a UserReturnCode_et
    int registerUser(QString username, QString password);

    /// \brief This slot returns the current ControlBox status
    /// \return Returns a cast to an integer from a ControlBoxStatus_et
    int getBoxStatus();

    void selectCalf(QString phyTag);

    void updateCalfList();
signals:
    /// \brief This signal is emitted when the current GUI menu is changed
    /// \param newMenu The menu that the GUI changed to
    void menuChanged(int newMenu);

    /// \brief This signal is emitted when the GUI menu should go back
    void menuBack();

    /// \brief This signal is emitted when the ControlBox status changes, ie. it goes from logged in to logged out, or
    /// vice-versa
    void boxStatusChanged(int newStatus);

    void clearCalfList();

    void addCalfToList(QString phyTag, float currentConsumption, float maxConsumption);

    void clearGraph();

    void clearSelection();

    void calfSelected(float maxConsumption, QString notes);

    void addPointToGraph(int timestamp, float volume);

};
#endif
