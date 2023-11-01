#ifndef CONTROLBOX_CNETWORKMANAGER_H
#define CONTROLBOX_CNETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <mutex>
#include <condition_variable>
#include "chttpform.h"

enum NetworkRequestType_et
{
    GET     = 0x0,
    POST    = 0x1,
    MAX
};

class CNetworkManager
{
public:
    ~CNetworkManager();
    static CNetworkManager* getInstance();
    static void killInstance();
    int executeRequest(const std::string& endpoint, const char* requestType,
                       CHttpForm& formData, QNetworkReply** reply);
private:
    CNetworkManager();
    std::string* m_requestNames;
    static CNetworkManager* m_instance;
    QNetworkAccessManager* m_networkAccessManager;
    QNetworkRequest* m_request;
    QNetworkReply* m_currentReply;
    std::condition_variable m_cv;
    std::mutex m_waitReady;
    std::mutex m_requestLock;
};


#endif
