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

class CNetworkManager : public CThread, QObject
{
public:
    ~CNetworkManager() override;
    static CNetworkManager* getInstance();
    void killInstance();
    int executeRequest(CHttpRequest& httpRequest);
private:
    CNetworkManager();
    void* run(void* args) override;
    void internalExecute();
    static CNetworkManager* m_instance;
    std::atomic<bool> m_keepRunning{};
    CHttpRequest* m_currentRequest;
    int m_currentReturn;
    SemaphoreHandle_st m_pendingSem;
    SemaphoreHandle_st m_readySem;
    QNetworkAccessManager* m_networkAccessManager;
    QNetworkRequest* m_request;
    std::condition_variable m_cv;
    std::mutex m_requestLock;
    std::mutex m_mtx;
};


#endif
