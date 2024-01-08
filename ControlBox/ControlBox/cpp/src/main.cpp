#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"
#include <iostream>


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

    return QApplication::exec();
}
