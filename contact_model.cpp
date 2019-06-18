#include "contact_model.h"
#include "chat_base.h"
#include "c2c/util.h"

#include <cstdlib>
#include <ctime>

#include <QDateTime>

contact_model::contact_model()
    : QAbstractListModel(nullptr)
    , data_("chat")
{
    size_t off = 0;
    c2c::cxx::result<std::string>(self_contact_id_).on(data_.get(pfx::SELF_CONTACT_ID), off);
    c2c::cxx::db db = data_.at(pfx::CONTACT);
    for(bool r=db.first(); r; r=db.next())
    {
        std::string k; contact_info c;
        off = 0; c2c::cxx::result<std::string>(k).on(db.key(), off);
        off = 0; c2c::cxx::result<contact_info>(c).on(db.val(), off);
        map_.push_back({k,c.tm, false});
    }
    std::sort(map_.begin(), map_.end(), [](const _C_& c1, const _C_& c2){return c1.tm > c2.tm; });
}

contact_model::~contact_model()
{
}

bool contact_model::add(const QByteArray& qid, const QByteArray& name)
{
    contact_info c;
    c2c::cxx::db db = data_.at(pfx::CONTACT);
    std::string id;
    c2c::hex2bin(qid.data(), id);
    std::string raw = db.get(id);
    if(raw.empty())
    {
        c.name = name.data();
        const char *color = random_color();
        c.ab_background.assign(color, 3);
        c.tm = time(nullptr);
        if(0.3*uint8_t(color[0])+0.59*uint8_t(color[1])+0.11*uint8_t(color[2]) > 128.0)
            c.ab_color.assign("\x00\x00\x00", 3);
        else
            c.ab_color.assign("\xFF\xFF\xFF", 3);
        c.type = contact_info::USER;
        db.put(id, c);
        beginInsertRows(index(0,0), 0, int(map_.size()));
        map_.push_back({id, c.tm, false});
        std::sort(map_.begin(), map_.end(), [](const _C_& c1, const _C_& c2){return c1.tm > c2.tm; });
        endInsertRows();
        return true;
    }
    return false;
}

bool contact_model::del(const QByteArray& qid)
{
    c2c::cxx::db db = data_.at(pfx::CONTACT);
    std::string id;
    c2c::hex2bin(qid.data(), id);
    if(db.del(id)) {
        for(size_t n=0; n<map_.size(); n++) {
            if(map_[n].id==id) {
                map_.erase(map_.begin()+int(n));
                emit dataChanged(index(0,0), index(int(map_.size())-1,0));
            }
        }
        return true;
    }
    return false;
}

QByteArray contact_model::self_contact_id()
{
    return c2c::bin2hex(self_contact_id_).c_str();
}

int contact_model::rowCount(const QModelIndex &) const
{
    return int(map_.size());
}

QHash<int, QByteArray> contact_model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ID] = "id";
    roles[ALIAS] = "alias";
    roles[AB] = "ab";
    roles[AB_COLOR] = "ab_color";
    roles[AB_BACKGROUND] = "ab_background";
    roles[UNREAD_CNT] = "unread_cnt";
    roles[TM] = "tm";
    roles[JOINED] = "joined";
    return roles;
}

QVariant contact_model::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && index.row() < int(map_.size()))
    {
        c2c::cxx::db db = data_.at(pfx::CONTACT);
        const _C_& cont = map_[size_t(index.row())];

        contact_info c; size_t off = 0;
        c2c::cxx::result<contact_info>(c).on(db.get(cont.id), off);

        auto alias = [&cont](const contact_info& c) -> std::string {
            return !c.alias.empty() ? c.alias.c_str() : (
                !c.name.empty() ? c.name.c_str() : c2c::bin2hex(cont.id)
            );
        };

        switch (role)
        {
            case ID: return c2c::bin2hex(cont.id).c_str();
            case ALIAS: return alias(c).c_str();
            case AB: return alias(c).substr(0,2).c_str();
            case AB_COLOR: return ("#"+c2c::bin2hex(c.ab_color)).c_str();
            case AB_BACKGROUND: return ("#"+c2c::bin2hex(c.ab_background)).c_str();
            case TM: return QDateTime().fromSecsSinceEpoch(c.tm).toString("dd.MM.yyyy hh:mm");
            case JOINED: return cont.joined;
            case UNREAD_CNT: {
                int c = 0; size_t off = 0;
                c2c::cxx::result<int>(c).on(data_.at(pfx::MESSAGE_CNT).get(cont.id), off);
                int r = 0; off = 0;
                c2c::cxx::result<int>(r).on(data_.at(pfx::MESSAGE_READED).get(cont.id), off);
                return c>r ? c-r : 0;
            }
        }
    }

    return QVariant();
}

bool contact_model::changeAlias(int row, const QByteArray &value)
{
    if (row >= 0 && row < int(map_.size()))
    {
        _C_& cont = map_[size_t(row)];
        contact_info c; size_t off = 0;
        c2c::cxx::result<contact_info>(c).on(data_.get(pfx::CONTACT, cont.id), off);
        c.alias = value.toStdString();
        data_.put(pfx::CONTACT, cont.id, c);
        emit dataChanged(index(row, 0), index(row, 0));
        return true;
    }
    return false;
}

void contact_model::on_join(std::string id)
{
    for(size_t n=0; n<map_.size(); n++)
    {
        _C_& cont = map_[n];
        if(cont.id == id)
        {
            cont.joined = true;
            emit dataChanged(index(int(n), 0), index(int(n), 0));
            return;
        }
    }

    beginResetModel();
    contact_info c; size_t off = 0;
    c2c::cxx::result<contact_info>(c).on(data_.get(pfx::CONTACT, id), off);
    map_.push_back({id, c.tm, true });
    std::sort(map_.begin(), map_.end(), [](const _C_& c1, const _C_& c2){return c1.tm > c2.tm; });
    endResetModel();
}

void contact_model::on_leave(std::string id)
{
    for(size_t n=0; n<map_.size(); n++)
    {
        _C_& cont = map_[n];
        if(cont.id == id)
        {
            cont.joined = false;
            emit dataChanged(index(int(n),0),index(int(n),0));
            break;
        }
    }
}

void contact_model::on_change(std::string id)
{
    for(size_t n=0; n<map_.size(); n++)
    {
        _C_& cont = map_[n];
        if(cont.id == id)
        {
            emit dataChanged(index(int(n),0),index(int(n),0));
            break;
        }
    }
}
