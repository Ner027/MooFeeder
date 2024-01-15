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
                    "selfName"  : station.name,
                    "hwId"      : getFormatedHwId(station.hw_id),
                    "phyId"     : station.hw_id
                };

                stationList.append(newObject);
            }
        }

        function clearTexts()
        {
            tempText.titleText    = "Temperature";
            batteryText.titleText = "Battery";
            macAddrText.titleText = "MAC Address";
            netAddrText.titleText = "Network Address";
            statusText.titleText  = "Status";
            nameText.titleText    = "Name";
        }

        function onClearStationList()
        {
            stationList.clear();
            clearTexts();
        }

        function onStationSelected(temp, bat, netAddr, stName, macAddr, status)
        {
            statusText.titleText     = "Status: " + status;
            macAddrText.titleText    = "MAC Address:" + getFormatedHwId(macAddr);
            nameText.titleText       = stName;

            if (status === "Connected")
            {
                tempText.titleText       = "Temperature: " + temp + "ºC";
                batteryText.titleText    = "Battery: " + bat + "v";
                netAddrText.titleText    = "Network Address: " + netAddr;
            }
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


            Item
            {
                id: rightContainer

                anchors.centerIn: parent

                width: rightPanel.width * 0.8
                height: rightPanel.height * 0.6

                BreakBar
                {
                    id: detailsBrakeBar
                    titleText: "Details"

                    anchors
                    {
                        horizontalCenter: parent.horizontalCenter
                        top: parent.top
                    }
                }

                TextIcon
                {
                    id: descriptionText

                    titleText: "Description"
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: detailsBrakeBar.bottom
                        topMargin: 10
                    }
                }

                TextIcon
                {
                    id: nameText

                    titleText: "Name"
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: descriptionText.bottom
                        topMargin: 10
                    }
                }

                BreakBar
                {
                    id: netInfo
                    titleText: "Network Information"

                    anchors
                    {
                        horizontalCenter: parent.horizontalCenter
                        top: nameText.bottom
                        topMargin: 20
                    }
                }

                TextIcon
                {
                    id: statusText

                    titleText: "Status: "
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: netInfo.bottom
                        topMargin: 10
                    }
                }

                TextIcon
                {
                    id: netAddrText

                    titleText: "Network Address: "
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: statusText.bottom
                        topMargin: 10
                    }
                }

                TextIcon
                {
                    id: macAddrText

                    titleText: "MAC Address: "
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: netAddrText.bottom
                        topMargin: 10
                    }
                }

                BreakBar
                {
                    id: sensorBreakBar
                    titleText: "Sensor Information"

                    anchors
                    {
                        horizontalCenter: parent.horizontalCenter
                        top: macAddrText.bottom
                        topMargin: 20
                    }
                }

                TextIcon
                {
                    id: tempText

                    titleText: "Temperature: "
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: sensorBreakBar.bottom
                        topMargin: 10
                    }
                }


                TextIcon
                {
                    id: batteryText

                    titleText: "Battery: "
                    iconImage.source:  "../../resources/images/Rfid.png"

                    anchors
                    {
                        left: detailsBrakeBar.left
                        top: tempText.bottom
                        topMargin: 10
                    }
                }
            }
        }
    }
}
