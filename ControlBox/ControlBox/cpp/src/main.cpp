#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"
#include "../inc/entities/ccalf.h"
#include <iostream>
#include <memory>

std::string exec(const char* cmd)
{
    char buffer[128];
    uint8_t rdIdx = 0;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(&buffer[rdIdx], (128 - rdIdx), pipe.get()))
    {
        result += buffer;
    }

    return result;
}

struct NetworkInfo
{
    std::string ssid;
    std::string authType;
    int32_t signalStrength;
};

std::vector<NetworkInfo> parseScanMessage(std::string src)
{
    std::string line, key;
    size_t currentPosition = 0;
    NetworkInfo networkInfo;
    std::vector<NetworkInfo> ret;

    qDebug() << src.c_str();

    currentPosition = src.find('\n');
    while (currentPosition != std::string::npos)
    {
        currentPosition = src.find('\n');

        line = src.substr(0, currentPosition);
        src = src.substr(currentPosition + 1);

        if (line.find("ESSID") != std::string::npos)
        {
            networkInfo.ssid = line.substr(line.find(':') + 1);
        }
        else if(line.find("Signal level") != std::string::npos)
        {
            networkInfo.signalStrength = (int32_t) strtol(line.substr(line.find("l=") + 1).c_str(), nullptr, 10);
        }
        else if (line.find("Authentication Suites") != std::string::npos)
        {
            networkInfo.authType = line.substr(line.find(':') + 1);
            ret.push_back(networkInfo);
        }
    }

    return ret;
}

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QQmlApplicationEngine qmlEngine;
    QQuickView view(&qmlEngine, nullptr);

    view.rootContext()->setContextProperty("QmlInterface", CQmlInterface::getInstance());
    view.setSource(QUrl("qrc:/qml/Main.qml"));
    view.show();

    /*
    auto ret = parseScanMessage(exec("iwlist wlo1 scan | grep -E \"ESSID|Signal level|Authentication Suites\""));

    for (const auto &item: ret)
    {
        qDebug() << "SSID: " << item.ssid.c_str();
        qDebug() << "AuthType: " << item.authType.c_str();
        qDebug() << "Signal Strength: " << item.signalStrength;
    }

    //CControlBox::getInstance()->executeLogin("user", "mooFeeder");
    //qDebug() << "Session Token: " << CControlBox::getInstance()->getSessionToken().c_str();
    */

    return QApplication::exec();
}
