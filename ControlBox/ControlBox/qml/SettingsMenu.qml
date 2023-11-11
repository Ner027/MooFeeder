import QtQuick 2.15
import QtQuick.Layouts 1.3
import "Components"

Item
{
    SwipeListener
    {
        onSwipe: () => {QmlInterface.goBack();}
    }

    Rectangle
    {
        anchors.fill: parent
        color: "#F5F5F5"

        RowLayout
        {
            anchors.centerIn: parent

            width: parent.width * 0.8

            property int itemCount: 4
            spacing: (width - shutdownButton.width * itemCount) / (itemCount - 1)

            MenuButton
            {
                id: shutdownButton

                titleText: "Shutdown"
                imageSource: "../resources/images/Shutdown.png"


                clickArea.onClicked: QmlInterface.shutdownBox()
            }

            MenuButton
            {
                id: rebootButton

                titleText: "Reboot"
                imageSource: "../resources/images/Reboot.png"

                clickArea.onClicked: QmlInterface.rebootBox()
            }

            MenuButton
            {
                id: wifiButton

                titleText: "WiFi"
                imageSource: "../resources/images/Wifi.png"

                clickArea.onClicked: QmlInterface.rebootBox()
            }

            MenuButton
            {
                id: loginButton

                titleText: "Login"
                imageSource: "../resources/images/Login.png"

                clickArea.onClicked: QmlInterface.changeToMenu(Types.GuiMenuType.LOGIN_MENU)
            }
        }
    }
}
