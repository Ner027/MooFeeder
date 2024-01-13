#ifndef CONTROLBOX_CNETWORKMANAGER_H
#define CONTROLBOX_CNETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <mutex>
#include <condition_variable>
#include "chttpform.h"
#include "chttprequest.h"
#include "networktypes.h"
#include "ptwrapper/cthread.h"
#include "csafequeue.hpp"
#include "oswrapper/oswrapper.h"

static std::string g_verbLut[] =
                        {"GET",
                         "HEAD",
                         "POST",
                         "PUT",
                         "DELETE",
                         "CONNECT",
                         "OPTIONS",
                         "TRACE",
                         "PATCH"};

class CNetworkManager
{
public:
    ~CNetworkManager();
    static CNetworkManager* getInstance();
    void killInstance();
    int executeRequest(CHttpRequest& httpRequest);
private:
    CNetworkManager();
    static CNetworkManager* m_instance;
    QNetworkAccessManager* m_networkAccessManager;
    QNetworkRequest* m_request;
    CMutex m_requestMutex;
};

#endif
