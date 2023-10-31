import QtQuick 2.15

Item
{
    id: mainContainer
    width: 1024
    height: 600
    implicitHeight: height
    implicitWidth: width
    visible: true

    Text
    {
        id: name
        anchors.fill: parent
        text: "Hello World"
    }
}
