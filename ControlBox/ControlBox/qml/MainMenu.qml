import QtQuick 2.15
import QtQuick.Layouts 1.3
import "Components"

Item
{
    Rectangle
    {
        anchors.fill: parent
        color: "#F5F5F5"

        RowLayout
        {
            anchors.centerIn: parent

            width: parent.width * 0.8

            property int itemCount: 4
            spacing: (width - monitorButton.width * itemCount) / (itemCount - 1)

            MenuButton
            {
                id: monitorButton

                titleText: "Calf Monitor"
                imageSource: "../resources/images/Calf.png"

                clickArea.onClicked: QmlInterface.changeToMenu(Types.GuiMenuType.MONITOR_MENU);
            }

            MenuButton
            {
                id:stationButton

                titleText: "Feeding Stations"
                imageSource: "../resources/images/FeedingStation.png"

                clickArea.onClicked: QmlInterface.changeToMenu(Types.GuiMenuType.STATION_MENU);
            }

            MenuButton
            {
                id: boxButton

                titleText: "Control Box"
                imageSource: "../resources/images/Box.png"

                clickArea.onClicked: QmlInterface.changeToMenu(Types.GuiMenuType.BOX_MENU);
            }

            MenuButton
            {
                id: settingsButton

                titleText: "System Settings"
                imageSource: "../resources/images/Settings.png"

                clickArea.onClicked: QmlInterface.changeToMenu(Types.GuiMenuType.SETTINGS_MENU);
            }
        }
    }
}
