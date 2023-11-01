import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import "Components"
Item
{
    property var popupData : null

    function getPopupText(retCode, actionType)
    {
        switch (retCode)
        {
            case Types.LoginReturnCode.USER_LOGGED:
                return actionType + " sucesseful!";
            case Types.LoginReturnCode.INVALID_USER:
                return "Invalid User!";
            case Types.LoginReturnCode.INVALID_PASSWORD:
                return "Invalid Passowrd!";
            case Types.LoginReturnCode.USER_EXISTS:
                return "User already exists!";
            case Types.LoginReturnCode.USER_NOT_FOUND:
                return "User not found!";
            case Types.LoginReturnCode.WRONG_PASSWORD:
                return "Wrong password provided!"
            default:
                return "Internal Error!";
        }
    }

    SwipeListener
    {
        onSwipe: () => {QmlInterface.goBack();}
    }

    Popup
    {
        id: eventPopup

        anchors.centerIn: parent
        width: parent.width * 0.4
        height: parent.height * 0.6

        modal: true

        closePolicy: Popup.CloseOnPressOutside

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

    Rectangle
    {
        anchors.fill: parent
        color: "#F5F5F5"

        Item
        {
            id: itemContainer

            anchors.centerIn: parent
            width: parent.width * 0.4
            height: usernameField.height + passwordField.height + loginButton.height + registerButton.height

            TextField
            {
                id: usernameField

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top

                width: parent.width
                font.pointSize: 24
                placeholderText: "Username"
                placeholderTextColor: "#E0E0E0"
                selectedTextColor: "#424242"

                validator: RegExpValidator
                {
                    regExp: /^[a-zA-Z0-9_.-]*$/
                }
            }

            TextField
            {
                id: passwordField

                anchors
                {
                    top: usernameField.bottom
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 20
                }

                width: parent.width
                font.pointSize: 24
                placeholderText: "Password"
                placeholderTextColor: "#E0E0E0"
                selectedTextColor: "#424242"
                echoMode: TextInput.Password

                validator: RegExpValidator
                {
                    regExp: /^[a-zA-Z0-9_.-]*$/
                }
            }

            RoundedButton
            {
                id: loginButton

                anchors
                {
                    top: passwordField.bottom
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 20
                }

                width: parent.width
                height: 60

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
                id: registerButton

                anchors
                {
                    top: loginButton.bottom
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 20
                }

                width: parent.width
                height: 60

                titleText: "Register"

                clickArea.onClicked:
                {
                    console.log("Username: " + usernameField.text);
                    console.log("Password: " + passwordField.text);
                }
            }
        }
    }
}
