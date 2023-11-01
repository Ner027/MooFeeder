import QtQuick 2.15
import QtQuick.Controls 2.5

Item
{
    id: mainContainer
    width: 1024
    height: 600
    implicitHeight: height
    implicitWidth: width
    visible: true


    Connections
    {
        target: QmlInterface

        function onMenuChanged(newMenu)
        {
            switch (newMenu)
            {
                case Types.GuiMenuType.SETTINGS_MENU:
                    stackView.push(settingsMenu);
                    break;
                case Types.GuiMenuType.LOGIN_MENU:
                    stackView.push(loginMenu);
                    break;
            }
        }

        function onMenuBack()
        {
            stackView.pop();
        }
    }


    property Item mainMenu: MainMenu{}
    property Item settingsMenu: SettingsMenu{}
    property Item loginMenu: LoginMenu{}

    StackView
    {
        id: stackView
        anchors.fill: parent
        initialItem: mainMenu
    }

}
