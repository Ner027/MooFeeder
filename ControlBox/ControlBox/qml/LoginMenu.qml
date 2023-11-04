import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.VirtualKeyboard 2.4

import "Components"
Item
{
    property var popupData : null

    function getPopupText(retCode, actionType)
    {
        switch (retCode)
        {
            case Types.UserReturnCode.USER_OK:
                return actionType + " sucesseful!";
            case Types.UserReturnCode.INVALID_USER:
                return "Invalid User!";
            case Types.UserReturnCode.INVALID_PASSWORD:
                return "Invalid Passowrd!";
            case Types.UserReturnCode.USER_EXISTS:
                return "User already exists!";
            case Types.UserReturnCode.USER_NOT_FOUND:
                return "User not found!";
            case Types.UserReturnCode.WRONG_PASSWORD:
                return "Wrong password provided!"
            default:
                return "Internal Error!";
        }
    }

    SwipeListener
    {
        onSwipe: () => {QmlInterface.goBack();}
    }

    Connections
    {
        target: QmlInterface

        function onBoxStatusChanged(newStatus)
        {
            logoutButton.bEnable = (newStatus === Types.ControlBoxStatus.LOGGED_IN);
            loginButton.bEnable  = (newStatus === Types.ControlBoxStatus.LOGGED_OUT);
        }
    }

    Popup
    {
        id: eventPopup

        anchors.centerIn: parent
        width: parent.width * 0.4
        height: parent.height * 0.6

        modal: true

        closePolicy: Popup.CloseOnPressOutside

        enter: Transition
        {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
        }

        exit: Transition
        {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
        }

        Rectangle
        {
            anchors.fill: parent
            color: "#F5F5F5"

            Text
            {
                anchors.fill: parent
                text: (popupData === null) ? "" : popupData["title"]
                font.bold: false
                font.pointSize: 32
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }

    InputPanel
    {
        id: inputPanel
        z: 1
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Rectangle
    {
        anchors.fill: parent
        color: "#F5F5F5"

        ColumnLayout
        {
            anchors.centerIn: parent
            width: parent.width * 0.4
            spacing: 20

            TextField
            {
                id: usernameField

                font.pointSize: 24
                placeholderText: "Username"
                placeholderTextColor: "#E0E0E0"
                selectedTextColor: "#424242"

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                validator: RegExpValidator
                {
                    regExp: /^[a-zA-Z0-9_.-]*$/
                }
            }

            TextField
            {
                id: passwordField

                font.pointSize: 24
                placeholderText: "Password"
                placeholderTextColor: "#E0E0E0"
                selectedTextColor: "#424242"
                echoMode: TextInput.Password

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                validator: RegExpValidator
                {
                    regExp: /^[a-zA-Z0-9_.-]*$/
                }
            }

            RoundedButton
            {
                id: loginButton

                height: 60
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                titleText: "Login"

                clickArea.onClicked: () =>
                {
                    var ret = QmlInterface.loginUser(usernameField.text, passwordField.text);
                    popupData = {"title": getPopupText(ret, "Login")};
                    eventPopup.open();
                }
            }

            RoundedButton
            {
                height: 60

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                titleText: "Register"

                clickArea.onClicked:
                {
                    var ret = QmlInterface.registerUser(usernameField.text, passwordField.text);
                    popupData = {"title": getPopupText(ret, "Register")};
                    eventPopup.open();
                }
            }


            RoundedButton
            {
                id: logoutButton

                height: 60

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                titleText: "Logout"

                bEnable: false

                clickArea.onClicked:
                {
                    QmlInterface.logoutUser();
                    usernameField.text = "";
                    passwordField.text = "";
                }
            }
        }
    }
}
