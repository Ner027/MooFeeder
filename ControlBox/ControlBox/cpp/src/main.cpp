#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"
#include <iostream>
#include "serial/serial.h"
#include "LoRa/phy/common/rn2483/rn2483.h"


int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QApplication app(argc, argv);
    QQmlApplicationEngine qmlEngine;
    QQuickView view(&qmlEngine, nullptr);

    view.rootContext()->setContextProperty("QmlInterface", CQmlInterface::getInstance());
    view.setSource(QUrl("qrc:/qml/Main.qml"));
    view.show();

    serial_port_st port;

    serial_open("/dev/ttyUSB0", B57600, &port);


    return QApplication::exec();
}
