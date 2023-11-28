import QtQuick 2.15
import QtCharts 2.3
import QtQuick.Layouts 1.3
import "Components"

Item
{

    function getFormatedHwId(hwId)
    {
        let formatedHwId = "";

        for(let i = 0; i < hwId.length; i++)
        {
            formatedHwId += hwId[i].toUpperCase();

            if (((i % 2) !== 0) && (i !== hwId.length - 1))
                formatedHwId += ":";
        }

        return formatedHwId;
    }

    SwipeListener
    {
        anchors.fill: parent
        onSwipe: () => {QmlInterface.goBack();}
    }

    Connections
    {
        target: QmlInterface

        function onSetStationList(sList)
        {
            for (let i = 0; i < sList.length; i++)
            {
                let station = sList[i];

                let newObject =
                {
                    "title"     : "Station " + i,
                    "selfName" : station.name,
                    "hwId"      : getFormatedHwId(station.hw_id)
                };

                stationList.append(newObject);
            }
        }

        function onClearStationList()
        {
            stationList.clear();
        }
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
                id: listView
                anchors.fill: parent
                spacing: 16
                delegate: StationDelegate{}
                model: ListModel {id: stationList}

                onFlickStarted: () => {QmlInterface.updateStationList();}
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
