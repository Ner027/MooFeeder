import QtQuick 2.15

//NOTE: When using this component together with another MouseArea,
//make sure this one is evaluated first, by placing it before other areas
Item
{
    enum Direction
    {
        Left,
        Right
    }

    property var onSwipe: null
    property var direction: SwipeListener.Direction.Right
    property int minDelta: 75

    anchors.fill: parent

    MouseArea
    {
        anchors.fill: parent
        z: 0

        property int initialX;

        onPressed:
        {
            initialX = mouse.x;
        }

        onReleased:
        {
            var posDelta = mouse.x - initialX;

            if(direction === SwipeListener.Direction.Left)
                posDelta *= -1;

            if(posDelta < minDelta)
                return;

            if(onSwipe !== null)
                onSwipe();
        }
    }
}
