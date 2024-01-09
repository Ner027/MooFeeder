#ifndef CONTROLBOX_CHTTPREQUEST_H
#define CONTROLBOX_CHTTPREQUEST_H

#include <QNetworkReply>
#include <QJsonObject>
#include "chttpform.h"
#include "networktypes.h"


class CHttpRequest
{
public:
    QNetworkReply* m_reply;
    CHttpForm m_formData;
    CHttpRequest();
    CHttpRequest(std::string endpoint, HttpVerb_et verb);
    std::string& getEndpoint();
    HttpVerb_et getVerb();
    HttpStatusCode_et getStatus() const;
    int execute();
    int getJsonData(QJsonObject* jsonObject);
private:
    bool m_ready;
    std::string m_endpoint;
    HttpVerb_et m_verb;
};

#endif
