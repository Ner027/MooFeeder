import QtQuick 2.15
import "Components"

Item
{
    SwipeListener
    {
        anchors.fill: parent
        onSwipe: () => {QmlInterface.goBack();}
    }

    Rectangle
    {
        anchors.fill: parent
        color: "#F5F5F5"

        Item
        {
            id: leftPanel

            width: parent.width * 0.4

            anchors
            {
                top: parent.top
                bottom: parent.bottom
                left: parent.left

                topMargin: 10
                bottomMargin: 10
                leftMargin: 10
                rightMargin: 10
            }


            ListView
            {
                anchors.fill: parent
                spacing: 16
                delegate: CalfDelegate{}
                model: ListModel
                {
                    ListElement
                    {
                        title: "Calf 0"
                        rfidTag: "DE:AD:C0:DE"
                        milkVolume: "7/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 1"
                        rfidTag: "FA:FE:DE:AD"
                        milkVolume: "3/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 2"
                        rfidTag: "00:C0:FF:EE"
                        milkVolume: "9/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 3"
                        rfidTag: "FE:ED:C0:DE"
                        milkVolume: "5/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 4"
                        rfidTag: "DE:AD:BE:EF"
                        milkVolume: "15/15 L"
                    }
                }
            }
        }

        Item
        {
            id: rightPanel

            anchors
            {
                top: parent.top
                bottom: parent.bottom
                left: leftPanel.right
                right: parent.right
            }

        }
    }
}
