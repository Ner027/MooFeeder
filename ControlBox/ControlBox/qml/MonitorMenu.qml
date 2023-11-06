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

        Text
        {
            anchors.fill: parent
            font.pointSize: 42
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: "Monitor"
        }
    }
}
