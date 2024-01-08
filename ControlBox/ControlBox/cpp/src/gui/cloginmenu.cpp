#include "gui/cloginmenu.h"

CloudReturnCode_et CLoginMenu::loginUser(QString &username, QString &password)
{
    return m_controlBoxInstance->executeLogin(username.toStdString(), password.toStdString());
}

void CLoginMenu::logoutUser()
{
    m_controlBoxInstance->executeLogout();
}

CloudReturnCode_et CLoginMenu::registerUser(QString &username, QString &password)
{
    QJsonObject jObject;
    CHttpRequest request(ENDPOINT_REGISTER, HttpVerb_et::POST);

    //Assemble request
    request.m_formData.addField(FIELD_USER, username.toUtf8())
                      .addField(FIELD_PSW,  password.toUtf8());

    //Perform request
    int ret = request.execute();

    //Check if request executed correctly
    if (ret < 0)
        return INTERNAL_ERROR;

    //If unable to extract a JSON Object from the request response, it was corrupted
    if (request.getJsonData(&jObject) < 0)
        return INTERNAL_ERROR;

    //If registration was successful the request should return HTTP Code Created
    if (request.getStatus() != HttpStatusCode_et::Created)
    {
        //In case the request was not successful, check the cause, if it doesn't exist, the response was corrupted
        if (!jObject.contains(FIELD_CAUSE))
            return INTERNAL_ERROR;

        //Otherwise cast it to a CloudReturnCode_et and return it
        return static_cast<CloudReturnCode_et>(jObject[FIELD_CAUSE].toInt());
    }

    //TODO: Retrieve session token from registration and auto-login

    //If a new user was registered correctly return USER_OK
    return USER_OK;
}

CLoginMenu::CLoginMenu()
{
    m_controlBoxInstance = CControlBox::getInstance();
}

GuiMenuType_et CLoginMenu::getType()
{
    return LOGIN_MENU;
}

bool CLoginMenu::requiresPrivilegedAccess()
{
    return false;
}

