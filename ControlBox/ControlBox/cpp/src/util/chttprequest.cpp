#include <QJsonDocument>
#include "../../inc/util/chttprequest.h"
#include "../../inc/util/cnetworkmanager.h"

CHttpRequest::CHttpRequest(std::string endpoint, HttpVerb_et verb)
{
    m_endpoint = endpoint;
    m_verb = verb;
    m_ready = false;
}

int CHttpRequest::execute()
{
    int ret =  CNetworkManager::getInstance()->executeRequest(*this);

    if (ret >= 0)
        m_ready = true;

    return ret;
}

std::string &CHttpRequest::getEndpoint()
{
    return m_endpoint;
}

HttpVerb_et CHttpRequest::getVerb()
{
    return m_verb;
}

HttpStatusCode_et CHttpRequest::getStatus() const
{
    if (!m_ready)
        return CODE_ERROR;

    return static_cast<HttpStatusCode_et>(m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
}

int CHttpRequest::getJsonData(QJsonObject* jsonObject)
{
    if (!jsonObject)
        return -EINVAL;

    QJsonDocument jDoc = QJsonDocument::fromJson(m_reply->readAll());

    if (jDoc.isEmpty())
        return -ENODATA;

    *jsonObject = jDoc.object();

    return 0;
}

