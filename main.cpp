#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "contact_model.h"
#include "message_model.h"
#include "chat.h"
#include "thr.h"

#include <thread>

int main(int argc, char *argv[])
{
    std::shared_ptr<thr> t = std::make_shared<thr>(argc, argv);
    std::shared_ptr<chat::server> chat = t->chat();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qRegisterMetaType<std::string>();
    qmlRegisterUncreatableType<contact_model>("СontactListModel", 1, 0, "СontactListModel", "СontactListModel can't be instantiated directly");
    qmlRegisterUncreatableType<message_model>("MessageListModel", 1, 0, "MessageListModel", "MessageListModel can't be instantiated directly");

    contact_model c;
    message_model m;

    engine.rootContext()->setContextProperty("contact_model", &c);
    engine.rootContext()->setContextProperty("message_model", &m);

    QObject::connect(chat.get(), &chat::server::on_contact_join, &c, &contact_model::on_join);
    QObject::connect(chat.get(), &chat::server::on_contact_leave, &c, &contact_model::on_leave);
    QObject::connect(chat.get(), &chat::server::on_contact_change, &c, &contact_model::on_change);
    QObject::connect(chat.get(), &chat::server::on_message_received_from, &m, &message_model::on_received_from);
    QObject::connect(chat.get(), &chat::server::on_message_changed, &m, &message_model::on_changed);
    QObject::connect(&m, &message_model::do_send_to, chat.get(), &chat::server::do_send_to);
    QObject::connect(&m, &message_model::do_readed, chat.get(), &chat::server::do_readed);
    QObject::connect(&m, &message_model::on_contact_change, &c, &contact_model::on_change);

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
    [url,&t](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
        {
            qmlClearTypeRegistrations();

            t->stop();
            t.reset();

            QCoreApplication::exit(-1);
        }
    },
    Qt::QueuedConnection);

    engine.load(url);

    t->start();
    int rc = app.exec();

    qmlClearTypeRegistrations();

    t->stop();
    t.reset();

    return rc;
}
