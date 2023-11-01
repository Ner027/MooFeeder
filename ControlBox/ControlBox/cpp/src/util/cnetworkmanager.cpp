#include "../../inc/util/cnetworkmanager.h"
#include "../../inc/cloudinfo.h"
#include <sstream>
#include <QUrl>
#include <QDebug>
#include <iostream>
#include <thread>
#include <QEventLoop>

CNetworkManager* CNetworkManager::m_instance = nullptr;

CNetworkManager* CNetworkManager::getInstance()
{
    if (!m_instance)
        m_instance = new CNetworkManager;

    return m_instance;
}

void CNetworkManager::killInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

CNetworkManager::~CNetworkManager()
{
    delete m_networkAccessManager;
}

CNetworkManager::CNetworkManager()
{
    m_networkAccessManager = new QNetworkAccessManager;
    m_request = new QNetworkRequest;
}

int CNetworkManager::executeRequest(const std::string& endpoint,
                                    const char* requestType,
                                    CHttpForm &formData,
                                    QNetworkReply** reply)
{
    if (!requestType)
        return -EINVAL;

    m_requestLock.lock();

    std::stringstream urlBuilder;
    urlBuilder << CLOUD_URL << endpoint;

    QUrl fullUrl(QString::fromStdString(urlBuilder.str()));

    m_request->setUrl(fullUrl);
    QEventLoop loop;

    *reply = m_networkAccessManager->sendCustomRequest(*m_request,
                                                      requestType,
                                                      formData.getMultiPart());


    QObject::connect(*reply, SIGNAL(finished()), &loop, SLOT(quit()));

    loop.exec();

    m_requestLock.unlock();

    return 0;
}



