import QtQuick 2.0
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.3

Item
{
    property bool isSelected: false
    width: ListView.view.width
    height: 100

    Rectangle
    {
        id: itemContainer

        anchors.fill: parent
        color: (isSelected ? "#757575" : "#E0E0E0")


        Connections
        {
            target: QmlInterface


            function onClearStationSelection()
            {
                isSelected = false;
            }
        }

        MouseArea
        {
            anchors.fill: parent

            onClicked: () =>
                       {
                           //TODO: Call C++ select from here
                           isSelected = true;
                       }
        }

        Item
        {
            height: parent.height * 0.95
            width: parent.width * 0.95
            anchors.centerIn: parent

            BreakBar
            {
                id: breakBar
                anchors
                {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }

                titleText: model.title
            }


            TextIcon
            {
                id: hwIdText
                anchors
                {
                    top: breakBar.bottom
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 4
                }

                titleText: model.hwId
                iconImage.source:  "../../resources/images/Rfid.png"
            }

            TextIcon
            {
                id: nameText
                anchors
                {
                    top: hwIdText.bottom
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 4
                }

                titleText: model.selfName
                iconImage.source:  "../../resources/images/Milk.png"
            }
        }
    }

    DropShadow
    {
       anchors.fill: itemContainer
       cached: true
       horizontalOffset: 4
       verticalOffset: 4
       radius: 8.0
       samples: 16
       color: "#8b8b8b"
       source: itemContainer
    }
}
