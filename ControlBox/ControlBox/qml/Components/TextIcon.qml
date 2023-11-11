import QtQuick 2.0

Item
{
    property string titleText: ""
    property alias iconImage: selfImage

    width: parent.width
    height: selfImage.height

    Image
    {
        id: selfImage
        height: textContainer.height

        anchors.left: parent.left

        fillMode: Image.PreserveAspectFit
    }

    Text
    {
        id: textContainer

        anchors.left: selfImage.right
        anchors.leftMargin: 2

        text: titleText

        font.pointSize: 16
        font.bold: true
        color: "#424242"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }
}
