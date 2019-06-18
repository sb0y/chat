#ifndef CONTACT_H
#define CONTACT_H

#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QAbstractListModel>

#include "c2c/db.hpp"

#include "contact_base.h"
#include "colors.h"

namespace chat { class server; }

class contact_model : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Fields { ID=0, ALIAS, AB, AB_COLOR, AB_BACKGROUND, UNREAD_CNT, TM, JOINED };
    Q_ENUM(Fields)

    explicit contact_model();
    virtual ~contact_model() override;

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE bool add(const QByteArray& id, const QByteArray& name);
    Q_INVOKABLE bool del(const QByteArray& id);
    Q_INVOKABLE bool changeAlias(int row, const QByteArray& value);

    Q_INVOKABLE QByteArray self_contact_id();

    Q_SLOT void on_join(std::string id);
    Q_SLOT void on_leave(std::string id);
    Q_SLOT void on_change(std::string id);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    std::string self_contact_id_;

    c2c::cxx::db                    data_;
    typedef struct { std::string id; time_t tm; bool joined; } _C_;
    std::vector<_C_>   map_;
};

#endif
