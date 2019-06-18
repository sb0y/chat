#ifndef CHAT_H
#define CHAT_H

#include <QObject>

#include "contact_base.h"
#include "message_base.h"

#include "c2c/p2p.h"
#include "c2c/db.hpp"

#include <boost/asio/deadline_timer.hpp>

Q_DECLARE_METATYPE(std::string)

namespace chat {

class server : public QObject, public std::enable_shared_from_this<server>
{
    Q_OBJECT

    std::string data_dir_;

    std::shared_ptr<c2c::p2p::server> p2p_;

    std::vector<std::string> u_seeds_;
    std::string self_contact_id_;

    c2c::cxx::db data_;

    typedef std::set<c2c::connection_ptr, std::owner_less<c2c::connection_ptr>> connection_map;
    connection_map seeds_, peers_;

    typedef std::map<std::string, c2c::func_t> meth_map;
    meth_map meth_;

    typedef std::map<std::string, c2c::func_r_t> meth_r_map;
    meth_r_map meth_r_;

    boost::asio::deadline_timer tm_;
    size_t idle_;

    void __open(c2c::connection_ptr con);
    void __close(c2c::connection_ptr con);
    bool __call(c2c::connection_ptr con, const std::string& method, const c2c::var_t& params);
    bool __call_r(c2c::connection_ptr con, const std::string& method, const c2c::var_t& params, c2c::call_t reply);

    void __on_join(c2c::connection_ptr con, const std::string& id, const std::string& sig, bool rep);
    void __on_leave(c2c::connection_ptr con, const std::string& id, const std::string& sig);

    void on_join(const std::string& id);
    void on_leave(const std::string& id);

    void do_join();
    void do_leave();

    void on_receive(
            c2c::connection_ptr con,
            const std::string& from,
            const std::string& to,
            const std::string& message,
            int n_index,
            time_t tm);

    void on_sended(c2c::connection_ptr con, const std::string& to, const std::string& from, int n_index, time_t tm);
    void on_readed(c2c::connection_ptr con, const std::string& to, int n_index, time_t tm);

    bool __do_send_to(const std::string& peer, const message_info& m) const;
    bool __do_send_to(const std::string& peer, const std::string& cmd, const c2c::map_t& data) const;
    void __do_relay_to(const std::string& peer, uint64_t hop, const message_info& m) const;
    void __do_relay_to(const std::string& peer, uint64_t hop, const std::string& cmd, const c2c::map_t& data) const;

    void on_idle(boost::system::error_code const& ec);

public:
    static void set_options_desc(boost::program_options::options_description_easy_init& options);
    server(std::shared_ptr<c2c::p2p::server> p2p, boost::program_options::variables_map& vm);

    void start();
    void stop();

    Q_SLOT void do_send_to(std::string to, int n_index);

    Q_SLOT void do_sended(const std::string& peer, const std::string& to, int n_index, time_t tm) const;
    Q_SLOT void do_readed(const std::string& peer, const std::string& to, int n_index, time_t tm) const;

    std::shared_ptr<c2c::p2p::server> p2p();

signals:
    void on_contact_join(std::string id);
    void on_contact_leave(std::string id);
    void on_contact_change(std::string id);
    void on_message_received_from(std::string to, int n_index, std::string from);
    void on_message_changed(std::string to, int n_index);
};

}

#endif
