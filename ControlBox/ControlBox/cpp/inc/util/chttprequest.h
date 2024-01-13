#ifndef CONTROLBOX_CHTTPREQUEST_H
#define CONTROLBOX_CHTTPREQUEST_H

#include <QNetworkReply>
#include <QJsonObject>
#include "chttpform.h"
#include "networktypes.h"

class CHttpRequest
{
public:
    CHttpRequest(const std::string& endpoint, HttpVerb_et verb);
    [[nodiscard]] int execute();
    [[nodiscard]] std::string getEndpoint() const;
    [[nodiscard]] HttpVerb_et getVerb() const;
    [[nodiscard]] HttpStatusCode_et getStatus() const;
    [[nodiscard]] int getJsonData(QJsonObject& jsonObject) const;
    QNetworkReply* m_reply;
    CHttpForm m_formData;
private:
    bool m_ready;
    std::string m_endpoint;
    HttpVerb_et m_verb;
};

#endif
