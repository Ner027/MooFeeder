#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"
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

    std::cout << result << std::endl;

    return result;
}

std::vector<std::string> parseIwListMessage(std::string src)
{
    std::string line;
    std::vector<std::string> ret;
    size_t currentPosition = 0;

    currentPosition = src.find('\n');
    while (currentPosition != std::string::npos)
    {
        currentPosition = src.find('\n');

        line = src.substr(0, currentPosition);
        src = src.substr(currentPosition + 1);

        if (line.find("ESSID") == std::string::npos)
            continue;

        ret.push_back(line.substr(line.find(':') + 1));
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

    return QApplication::exec();

}
