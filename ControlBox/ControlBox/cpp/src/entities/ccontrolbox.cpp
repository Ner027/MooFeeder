#include <QJsonObject>
#include <QJsonDocument>
#include "../../inc/entities/ccontrolbox.h"

CControlBox* CControlBox::m_instance = nullptr;

CControlBox::CControlBox()
{
    //When created the ControlBox is logged out
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
    if (!m_instance)
        return;

    delete m_instance;
    m_instance = nullptr;
}

CloudReturnCode_et CControlBox::executeLogin(const std::string& username, const std::string& password)
{
    QJsonObject jObject;
    CHttpRequest request(ENDPOINT_LOGIN, HttpVerb_et::GET);

    //Assemble the request
    request.m_formData.addField(FIELD_USER, QByteArray(username.c_str()))
                    .addField(FIELD_PSW, QByteArray(password.c_str()));

    int ret = request.execute();

    //Check if the request executed properly
    if (ret < 0)
        return INTERNAL_ERROR;

    //If unable to extract a JSON Object from the request response, it was corrupted
    if (request.getJsonData(&jObject) < 0)
        return INTERNAL_ERROR;

    //If logged in successful the request should return HTTP Code Accepted
    if (request.getStatus() != HttpStatusCode_et::Accepted)
    {
        //In case the request was not successful, check the cause, if it doesn't exist, the response was corrupted
        if (!jObject.contains(FIELD_CAUSE))
            return INTERNAL_ERROR;

        //Otherwise cast it to a CloudReturnCode_et and return it
        return static_cast<CloudReturnCode_et>(jObject[FIELD_CAUSE].toInt());
    }

    //If the login request was accepted but no session token was provided, the request response was corrupted
    if (!jObject.contains(FIELD_TOKEN))
        return INTERNAL_ERROR;

    //If all the above checks succeed, save the session token and update the ControlBox Status
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

    //TODO: Execute extra logout steps

    m_status = ControlBoxStatus_et::LOGGED_OUT;
}

std::string CControlBox::getSessionToken()
{
    return m_sessionToken;
}


