import QtQuick 2.15
import QtCharts 2.3
import QtQuick.Layouts 1.3
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
                anchors.fill: parent
                spacing: 16
                delegate: CalfDelegate{}
                model: ListModel
                {
                    ListElement
                    {
                        title: "Calf 0"
                        rfidTag: "DE:AD:C0:DE"
                        milkVolume: "7/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 1"
                        rfidTag: "FA:FE:DE:AD"
                        milkVolume: "3/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 2"
                        rfidTag: "00:C0:FF:EE"
                        milkVolume: "9/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 3"
                        rfidTag: "FE:ED:C0:DE"
                        milkVolume: "5/15 L"
                    }
                    ListElement
                    {
                        title: "Calf 4"
                        rfidTag: "DE:AD:BE:EF"
                        milkVolume: "15/15 L"
                    }
                }
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
                        anchors.fill: parent
                        antialiasing: true
                        legend.visible: false
                        backgroundColor: "#F5F5F5"
                        plotAreaColor: "#F5F5F5"


                        DateTimeAxis
                        {
                            id: axisX
                            tickCount: 5
                        }

                        AreaSeries
                        {
                            color: "#D2E8F8"
                            borderColor: "#80CAFF"
                            borderWidth: 2

                            upperSeries:
                            LineSeries
                            {
                                axisX: axisX
                                XYPoint { x: 1; y: 0 }
                                XYPoint { x: 2; y: 4 }
                                XYPoint { x: 3; y: 6 }
                                XYPoint { x: 4; y: 2.1 }
                                XYPoint { x: 5; y: 4.9 }
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
                            bottom: notesText.top
                            bottomMargin: 10
                        }

                        titleText: "Details"
                    }

                    TextIcon
                    {
                        id: notesText
                        anchors.centerIn: parent
                        titleText: "Notes"
                        iconImage.source: "../../resources/images/Milk.png"
                    }

                    TextIcon
                    {
                        id: consumptionText
                        anchors
                        {
                            horizontalCenter: parent.horizontalCenter
                            top: notesText.bottom
                            topMargin: 10
                        }

                        titleText: "Max Daily Consumption"
                        iconImage.source: "../../resources/images/Milk.png"
                    }
                }
            }
        }
    }
}
