#include <QJsonObject>
#include <QJsonDocument>
#include "../../inc/entities/ccontrolbox.h"

CControlBox* CControlBox::m_instance = nullptr;

CControlBox::CControlBox()
{
    m_networkManager = CNetworkManager::getInstance();
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

LoginReturnCode_et CControlBox::executeLogin(const std::string& username, const std::string& password)
{
    QNetworkReply* requestReply;
    CHttpForm formData;

    formData.addField(FIELD_USER, QByteArray(username.c_str()))
            .addField(FIELD_PSW, QByteArray(password.c_str()));

    m_networkManager->executeRequest(ENDPOINT_LOGIN, GET_REQUEST, formData, &requestReply);

    QJsonObject jObject = QJsonDocument::fromJson(requestReply->readAll()).object();

    if (jObject.isEmpty())
        return INTERNAL_ERROR;

    qDebug() << requestReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (requestReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 202)
    {
        if (!jObject.contains(FIELD_CAUSE))
            return INTERNAL_ERROR;

        return static_cast<LoginReturnCode_et>(jObject[FIELD_CAUSE].toInt());
    }

    if (!jObject.contains(FIELD_TOKEN))
        return INTERNAL_ERROR;

    m_sessionToken = jObject[FIELD_TOKEN].toString().toStdString();

    return USER_LOGGED;
}


