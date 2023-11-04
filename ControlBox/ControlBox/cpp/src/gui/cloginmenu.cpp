#include "../../inc/gui/cloginmenu.h"
#include "../../inc/entities/ccontrolbox.h"

UserReturnCode_et CLoginMenu::loginUser(QString &username, QString &password)
{
    return CControlBox::getInstance()->executeLogin(username.toStdString(), password.toStdString());
}

void CLoginMenu::logoutUser()
{
    CControlBox::getInstance()->executeLogout();
}

UserReturnCode_et CLoginMenu::registerUser(QString &username, QString &password)
{
    QJsonObject jObject;
    CHttpRequest request(ENDPOINT_REGISTER, HttpVerb_et::POST);

    request.m_formData.addField(FIELD_USER, username.toUtf8())
                      .addField(FIELD_PSW,  password.toUtf8());

    int ret = request.execute();

    if (ret < 0)
        return INTERNAL_ERROR;

    if (request.getJsonData(&jObject) < 0)
        return INTERNAL_ERROR;

    if (request.getStatus() != HttpStatusCode_et::Created)
    {
        if (!jObject.contains(FIELD_CAUSE))
            return INTERNAL_ERROR;

        return static_cast<UserReturnCode_et>(jObject[FIELD_CAUSE].toInt());
    }

    return USER_OK;
}

