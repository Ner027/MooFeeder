import QtQuick 2.15
import QtCharts 2.3
import QtQuick.Layouts 1.3
import "Components"

Item
{
    property bool initialized: false
    property int itemCount: 0

    function getFormatedTag(phyTag)
    {
        let shortTag = phyTag.slice(0, 16);
        let formatedTag = "";

        for(let i = 0; i < shortTag.length; i++)
        {
            formatedTag += shortTag[i].toUpperCase();

            if (((i % 2) !== 0) && (i !== shortTag.length - 1))
                formatedTag += ":";
        }

        return formatedTag;
    }

    SwipeListener
    {
        anchors.fill: parent
        onSwipe: () => {QmlInterface.goBack();}
    }

    Connections
    {
        target: QmlInterface

        function onClearCalfList()
        {
            calfList.clear();
        }

        function onAddCalfToList(phyTag, currentConsumption, maxConsumption)
        {
            calfList.append({"title": "Calf " + calfList.count,
                                "rfidTag" : getFormatedTag(phyTag),
                                "phyTag"  : phyTag,
                                "milkVolume": currentConsumption + "/" + maxConsumption + "L"});
        }

        function onSetCalfList(cList)
        {
            for (let i = 0; i < cList.length; i++)
            {
                let jObj = cList[i];
                onAddCalfToList(jObj.rfidTag, jObj.currentConsumption, jObj.maxConsumption);
            }
        }

        function onClearGraph()
        {
            initialized = false;
            itemCount = 0;
            xAxis.tickCount = 0;
            xAxis.max = new Date(0);
            xAxis.min = new Date(0);

            chart.setAxisX(xAxis);
            chart.setAxisY(yAxis);

            lineSeries.clear();
            lineSeries.axisX = xAxis;
            lineSeries.axisY = yAxis;

            chart.update();
        }

        function onAddPointToGraph(timestamp, volume)
        {
            let timestampMillis = timestamp * 1000;
            let dateObj = new Date(timestampMillis);

            if (!initialized)
            {
                xAxis.min = dateObj;
                initialized = true;
            }

            if (xAxis.max < dateObj)
            {
                xAxis.max = dateObj;
            }

            if (dateObj < xAxis.min)
            {
                xAxis.min = dateObj;
            }

            lineSeries.append(timestampMillis, volume);

            itemCount++;
            xAxis.tickCount = itemCount;

            chart.update();
        }

        function onCalfSelected(maxConsumption, notes)
        {
            consumptionText.titleText = "Max daily consumption: " + maxConsumption + "L"
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
                delegate: CalfDelegate{}
                model: ListModel {id: calfList}

                onFlickStarted: () => {QmlInterface.updateCalfList();}
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
                anchors.centerIn: parent
                width: parent.width * 0.8
                height: parent.height

                Item
                {
                    id: topRightPanel

                    height: parent.height * 0.7
                    anchors
                    {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }


                    ChartView
                    {
                        id: chart
                        anchors.fill: parent
                        antialiasing: true
                        legend.visible: false
                        backgroundColor: "#F5F5F5"
                        plotAreaColor: "#F5F5F5"

                        AreaSeries
                        {
                            color: "#D2E8F8"
                            borderColor: "#80CAFF"
                            borderWidth: 2

                            axisY:
                            ValueAxis
                            {
                                id: yAxis
                                labelFormat: "%.2fL"
                                max: 15
                                min: 0
                                tickCount: 5
                            }

                            axisX:
                            DateTimeAxis
                            {
                                id: xAxis
                                format: "dd/MM <br> hh:mm"
                            }

                            upperSeries:
                            LineSeries
                            {
                                id: lineSeries
                            }
                        }
                    }
                }

                Item
                {
                    id: bottomRightPanel

                    anchors
                    {
                        top: topRightPanel.bottom
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    BreakBar
                    {
                        id: breakBar
                        anchors
                        {
                            horizontalCenter: parent.horizontalCenter
                            bottom: consumptionText.top
                            bottomMargin: 10
                        }

                        titleText: "Details"
                    }

                    TextIcon
                    {
                        id: consumptionText
                        anchors.centerIn: parent
                        titleText: "Max daily consumption"
                        iconImage.source: "../../resources/images/Milk.png"
                    }
                }
            }
        }
    }
}
