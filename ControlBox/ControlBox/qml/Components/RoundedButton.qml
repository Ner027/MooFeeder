import QtQuick 2.0

Item
{
    property bool bEnable: true
    property alias clickArea: mouseArea
    property string titleText: ""

    height: 40
    width: 100

    Rectangle
    {
        anchors.fill: parent
        color: "#E0E0E0"
        radius: height * 0.15

        border
        {
            color: "#424242"
            width: 2
        }

        Text
        {
            anchors.fill: parent
            text: titleText
            font.pointSize: 24
            color: "#424242"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea
        {
            id: mouseArea
            anchors.fill: parent
            enabled: parent.parent.bEnable
        }

        opacity: parent.bEnable ? (mouseArea.pressed ? 0.5 : 1) : 0.5
    }
}
