#include "easylogging++.h"

#include "contact_base.h"
#include "message_model.h"
#include "chat_base.h"
#include "c2c/util.h"

#include <cstdlib>
#include <ctime>

#include <QDateTime>

message_model::message_model()
    : QAbstractListModel(nullptr)
    , data_("chat")
{
    size_t off = 0;
    c2c::cxx::result<std::string>(self_contact_id_).on(data_.get(pfx::SELF_CONTACT_ID), off);
}

message_model::~message_model()
{
}

int message_model::rowCount(const QModelIndex &) const
{
    int cnt = 0; size_t off = 0;
    c2c::cxx::result<int>(cnt).on(data_.at(pfx::MESSAGE_CNT, current_contact_).get(),off);
    return cnt;
}

QHash<int, QByteArray> message_model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MESSAGE] = "message";
    roles[TM] = "tm";
    roles[FROM] = "from";
    roles[FROM_NAME] = "from_name";
    roles[FROM_AB] = "from_ab";
    roles[FROM_AB_COLOR] = "from_ab_color";
    roles[FROM_AB_BACKGROUND] = "from_ab_background";
    roles[N_INDEX] = "n_index";
    roles[SENDED] = "sended";
    roles[READED] = "readed";
    return roles;
}

QVariant message_model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    c2c::cxx::db db = data_.at(pfx::MESSAGE, current_contact_);
    c2c::cxx::db cont = data_.at(pfx::CONTACT);

    int cnt = 0; size_t off = 0;
    c2c::cxx::result<int>(cnt).on(data_.at(pfx::MESSAGE_CNT, current_contact_).get(),off);

    if (index.row() < cnt)
    {
        if(db.first() && db.skip(index.row()))
        {
            int n_index = 0; std::string from; size_t off = 0;
            c2c::cxx::result<int, std::string>(n_index, from).on(db.key(), off);

            message_info m; off = 0;
            c2c::cxx::result<message_info>(m).on(db.val(), off);

            int r_cnt = 0; off = 0;
            c2c::cxx::result<int>(r_cnt).on(data_.at(pfx::MESSAGE_READED, current_contact_).get(),off);

            if(r_cnt <= n_index)
            {
                data_.at(pfx::MESSAGE_READED, current_contact_).put(n_index + 1);

                if(from != self_contact_id_)
                {
                    m.t.r = time(nullptr);
                    data_.at(pfx::MESSAGE, current_contact_, n_index, from).put(m);
                    emit do_readed(from,
                        current_contact_ == from ? self_contact_id_ : current_contact_, m.n, m.t.r);
                    emit on_contact_change(current_contact_);
                }
            }

            off = 0; contact_info c;
            c2c::cxx::result<contact_info>(c).on(cont.get(from),off);

            auto alias = [&from](const contact_info& c) -> std::string {
                return !c.alias.empty() ? c.alias.c_str() : (
                    !c.name.empty() ? c.name.c_str() : c2c::bin2hex(from)
                );
            };

            switch (role)
            {
                case FROM: return c2c::bin2hex(from).data();
                case MESSAGE: return m.m.c_str();
                case TM: return QDateTime().fromSecsSinceEpoch(m.t.c).toString("dd.MM.yyyy hh:mm");
                case FROM_NAME: return alias(c).c_str();
                case FROM_AB: return alias(c).substr(0,2).data();
                case FROM_AB_COLOR: return ("#"+c2c::bin2hex(c.ab_color)).data();
                case FROM_AB_BACKGROUND: return ("#"+c2c::bin2hex(c.ab_background)).data();
                case N_INDEX: return n_index;
                case SENDED: return bool(m.t.s > 0);
                case READED: return bool(m.t.r > 0);
            }
        }
    }
    return QVariant();
}

void message_model::append(const QString& message)
{
    if(current_contact_.size() > 0)
    {
        int cnt = 0; size_t off = 0;
        c2c::cxx::result<int>(cnt).on(data_.get(pfx::MESSAGE_CNT, current_contact_),off);
        c2c::cxx::db db = data_.at(pfx::MESSAGE);
        message_info m;
        m.m = message.toStdString();
        m.t.c = time(nullptr);
        m.t.s = 0;
        m.t.r = 0;
        m.n = cnt;
        beginInsertRows(QModelIndex(), cnt, cnt);
        db.put(current_contact_, cnt, self_contact_id_, m);
        data_.put(pfx::MESSAGE_CNT, current_contact_, cnt+1);
        endInsertRows();
        emit do_send_to(current_contact_, cnt);
    }
}

void message_model::on_received_from(std::string to, int n_index, std::string from)
{
    if((to == self_contact_id_ && from == current_contact_) || to == current_contact_)
    {
        beginInsertRows(QModelIndex(), n_index, n_index);
        endInsertRows();
    }
}

void message_model::on_changed(std::string to, int n_index)
{
    if(to == current_contact_)
        emit dataChanged(index(n_index,0), index(n_index,0));
}

QByteArray message_model::get_current_contact() const
{
    return c2c::bin2hex(current_contact_).c_str();
}

void message_model::set_current_contact(const QByteArray& contact)
{
    beginResetModel();
    c2c::hex2bin(contact.data(), current_contact_);
    endResetModel();
}
