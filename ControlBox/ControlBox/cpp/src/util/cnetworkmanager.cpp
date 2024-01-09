#include "../../inc/util/cnetworkmanager.h"
#include "../../inc/cloudtypes.h"
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
    m_keepRunning = false;
    delete m_instance;
    m_instance = nullptr;
}

CNetworkManager::~CNetworkManager()
{
    delete m_networkAccessManager;
}

CNetworkManager::CNetworkManager()
{
    m_keepRunning = true;
    m_networkAccessManager = new QNetworkAccessManager;
    m_request = new QNetworkRequest;
    SemaphoreInit(&m_pendingSem);
    SemaphoreInit(&m_readySem);

    this->start();
    this->detach();
}

int CNetworkManager::executeRequest(CHttpRequest& httpRequest)
{
    if (httpRequest.getVerb() >= HttpVerb_et::MAX)
        return -EINVAL;

    m_currentRequest = &httpRequest;

    SemaphoreGive(&m_pendingSem);
    SemaphoreTake(&m_readySem);

    return 0;
}

void* CNetworkManager::run(void* args)
{
    m_networkAccessManager->setParent(this);

    while (m_keepRunning)
    {
        SemaphoreTake(&m_pendingSem);

        std::stringstream urlBuilder;
        urlBuilder << CLOUD_URL << m_currentRequest->getEndpoint();

        QUrl fullUrl(QString::fromStdString(urlBuilder.str()));

        m_request->setUrl(fullUrl);
        QEventLoop loop;

        m_currentRequest->m_reply = m_networkAccessManager->sendCustomRequest(*m_request,
                                                                        g_verbLut[m_currentRequest->getVerb()].c_str(),
                                                                        m_currentRequest->m_formData.getMultiPart());

        QObject::connect(m_currentRequest->m_reply, SIGNAL(finished()), &loop, SLOT(quit()));

        loop.exec();

        m_requestLock.unlock();

        SemaphoreGive(&m_readySem);
    }

    return nullptr;
}





