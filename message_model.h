#ifndef MESSAGES_H
#define MESSAGES_H

#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QAbstractListModel>

#include "message_base.h"
#include "c2c/db.hpp"

namespace chat { class server; }

class message_model : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QByteArray current_contact READ get_current_contact WRITE set_current_contact NOTIFY current_contact_changed)

public:
    explicit message_model();
    virtual ~message_model();

    enum message_fields { MESSAGE, TM, FROM, FROM_NAME, FROM_AB, FROM_AB_COLOR, FROM_AB_BACKGROUND, N_INDEX, SENDED, READED };

    Q_ENUM(message_fields)

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE void append(const QString& message);

    Q_SLOT void on_received_from(std::string  to, int n_index, std::string from);
    Q_SLOT void on_changed(std::string  to, int n_index);

    QByteArray get_current_contact() const;
    void set_current_contact(const QByteArray& contact);

signals:
    void current_contact_changed();
    void do_send_to(std::string to, int n_index);
    void do_readed(const std::string& peer, const std::string& to, int n_index, time_t tm) const;
    void on_contact_change(std::string id) const;

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    c2c::cxx::db data_;
    std::string current_contact_;
    std::string self_contact_id_;
};

#endif
