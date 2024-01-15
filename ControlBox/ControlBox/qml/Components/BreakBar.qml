import QtQuick 2.0
import QtQuick.Layouts 1.3

Item
{
    property string titleText: ""

    width: parent.width
    height: textContainer.height + bar.height
    implicitHeight: height

    Layout.preferredHeight: height
    Layout.preferredWidth: width

    Text
    {
        id: textContainer
        text: titleText
        font.pointSize: 16
        font.bold: true
        color: "#424242"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }

    Rectangle
    {
        id: bar

        width: parent.width
        height: 2

        anchors.top: textContainer.bottom
        anchors.topMargin: 2

        color: "#424242"
    }

}
