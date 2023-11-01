import QtQml 2.3
import QtQml.Models 2.3
import QtQuick 2.0
import QtQuick.Layouts 1.3

Item
{
    property alias imageSource: itemImage.source
    property alias titleText: itemText.text
    property alias clickArea: mouseArea

    height: 200
    width: 150

    Rectangle
    {
        id: rectContainer

        width: parent.width
        height: width
        radius: width * 0.5

        color: "transparent"

        border
        {
            color: "#212121"
            width: 5
        }

        Image
        {
            id: itemImage

            height: parent.height * 0.8
            width: parent.width * 0.8

            anchors
            {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }
        }

        MouseArea
        {
            id: mouseArea
            anchors.fill: parent
        }

        opacity: mouseArea.pressed ? 0.5 : 1
    }


    Text
    {
        id: itemText

        width: itemImage.width
        anchors.top: rectContainer.bottom
        anchors.horizontalCenter: rectContainer.horizontalCenter

        font.pointSize: 24
        font.bold: true

        color: "#212121"

        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
