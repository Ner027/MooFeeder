#include <QGuiApplication>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QQmlApplicationEngine>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <ControlBox/control_box.h>
#include <Util/http_util.h>
#include <QDebug>
#include <QObject>

static QNetworkReply* reply;

void onReady()
{
    qDebug() <<  "Ready";
    qDebug() << reply->readAll();
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    CControlBox* box = CControlBox::getInstance();
    QNetworkAccessManager* networkManager = new QNetworkAccessManager();
    QNetworkRequest networkRequest(QUrl("http://localhost:8080/box/login"));

    CHttpForm httpForm;

    httpForm
        .addField("username", "andre")
        .addField("password", "mooFeeder");


    reply = networkManager->sendCustomRequest(networkRequest, "GET", httpForm.getMultiPart());

    QObject::connect(reply, &QNetworkReply::finished, onReady);

    return app.exec();
}
