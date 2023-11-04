#include <QJsonObject>
#include <QJsonDocument>
#include "../../inc/entities/ccontrolbox.h"

CControlBox* CControlBox::m_instance = nullptr;

CControlBox::CControlBox()
{
    m_status = LOGGED_OUT;
}

CControlBox* CControlBox::getInstance()
{
    if (!m_instance)
        m_instance = new CControlBox;

    return m_instance;
}

void CControlBox::killInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

UserReturnCode_et CControlBox::executeLogin(const std::string& username, const std::string& password)
{
    QJsonObject jObject;
    CHttpRequest request(ENDPOINT_LOGIN, HttpVerb_et::GET);

    request.m_formData.addField(FIELD_USER, QByteArray(username.c_str()))
                    .addField(FIELD_PSW, QByteArray(password.c_str()));

    int ret = request.execute();

    if (ret < 0)
        return INTERNAL_ERROR;

    if (request.getJsonData(&jObject) < 0)
        return INTERNAL_ERROR;

    if (request.getStatus() != HttpStatusCode_et::Accepted)
    {
        if (!jObject.contains(FIELD_CAUSE))
            return INTERNAL_ERROR;

        return static_cast<UserReturnCode_et>(jObject[FIELD_CAUSE].toInt());
    }

    if (!jObject.contains(FIELD_TOKEN))
        return INTERNAL_ERROR;

    m_sessionToken = jObject[FIELD_TOKEN].toString().toStdString();
    m_status = ControlBoxStatus_et::LOGGED_IN;

    return USER_OK;
}

ControlBoxStatus_et CControlBox::getStatus()
{
    return m_status;
}

void CControlBox::executeLogout()
{
    if (m_status != ControlBoxStatus_et::LOGGED_IN)
        return;

    //Execute extra logout steps

    m_status = ControlBoxStatus_et::LOGGED_OUT;
}


