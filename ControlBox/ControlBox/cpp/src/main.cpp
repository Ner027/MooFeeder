#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"


int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine qmlEngine;
    QQuickView view(&qmlEngine, nullptr);

    view.rootContext()->setContextProperty("QmlInterface", CQmlInterface::getInstance());
    view.setSource(QUrl("qrc:/qml/Main.qml"));
    view.show();

    return QGuiApplication::exec();
}