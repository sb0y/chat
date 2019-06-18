#include "easylogging++.h"

#include "chat_base.h"
#include "chat.h"
#include "colors.h"
#include "c2c/util.h"
#include "prefix_opt.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "sodium.h"

using namespace std::placeholders;

namespace chat {

void server::set_options_desc(boost::program_options::options_description_easy_init& options)
{
    options ("data-dir", boost::program_options::value<std::string>(), "path of data directory")
            ("chat-group-*", boost::program_options::value<std::vector<std::string>>(), "init chat groups")
            ("chat-seed", boost::program_options::value<std::vector<std::string>>(), "init seed list");
}

server::server(std::shared_ptr<c2c::p2p::server> p2p, boost::program_options::variables_map& vm)
    : p2p_(p2p), data_("chat"), tm_(p2p->get_io_service()), idle_(0)
{
    std::srand(uint32_t(std::time(nullptr)));

    if(vm.count("data-dir") > 0)
        data_dir_ = c2c::path_normalize(vm["root-dir"].as<std::string>());

    if(vm.count("chat-seed") > 0)
    {
        u_seeds_ = vm["chat-seed"].as<std::vector<std::string>>();
    }

    // read groups config
    struct __contact__ {
        std::string id, sk;
        std::vector<std::string> peers;
    };

    std::map<std::string, __contact__> group_list;
    BOOST_FOREACH(const p_options_type::value_type& p, get_p_options(vm, "chat-group-"))
    {
        std::vector<std::string> tokens;
        boost::split(tokens, p.first, boost::is_any_of("."));

        if(tokens.size() != 2)
        {
            LOG(ERROR) << "error chat group options: " << (tokens.size() > 0 ? tokens[0] : "");
            continue;
        }

        __contact__& c =  group_list[tokens[0]];

        if(p.second.size() > 0)
        {
            if(tokens[1] == "id")
            {
                c2c::hex2bin(p.second[0], c.id);
            }
            else if(tokens[1] == "sk")
            {
                c2c::hex2bin(p.second[0], c.sk);
            }
            else if(tokens[1] == "peer")
            {
                for(const std::string& h : p.second)
                {
                    std::string peer;
                    c2c::hex2bin(h, peer);
                    if(peer == self_contact_id_)
                        continue;
                    c.peers.push_back(peer);
                }
            }
        }
    }

    data_.begin();
    for(auto g : group_list)
    {
        contact_info c;
        std::string raw = data_.get(pfx::CONTACT, g.second.id);
        if(raw.empty())
        {
            const char *color = random_color();
            c.ab_background.assign(color, 3);
            c.tm = time(nullptr);
            if(0.3*uint8_t(color[0])+0.59*uint8_t(color[1])+0.11*uint8_t(color[2]) > 128.0)
                c.ab_color.assign("\x00\x00\x00", 3);
            else
                c.ab_color.assign("\xFF\xFF\xFF", 3);
            c.type = contact_info::GROUP;
        }
        else
        {
            size_t off = 0;
            c2c::cxx::result<contact_info>(c).on(raw, off);
        }
        c.name = g.first;

        data_.put(pfx::CONTACT, g.second.id, c);

        std::map<std::string, bool> del_map;
        c2c::cxx::db gdb = data_.at(pfx::GROUP_PEERS, g.second.id);
        if(gdb.first()) {
            for(bool b=true; b; b=gdb.next()) {
                std::string peer; size_t off = 0;
                c2c::cxx::result<std::string>(peer).on(gdb.key(), off);
                del_map[peer] = true;
            }
        }

        for(const std::string& peer : g.second.peers)
        {
            if(peer == self_contact_id_)
                continue;
            data_.put(pfx::GROUP_PEERS, g.second.id, peer, true);
            del_map[peer] = false;
        }

        // delete peers from group
        for(auto n : del_map)
            if(n.second)
                data_.del(pfx::GROUP_PEERS, g.second.id, n.first);
    }
    data_.commit();
    // read groups config finished

    c2c::hex2bin(p2p_->self_id(), self_contact_id_);
    data_.put(pfx::SELF_CONTACT_ID, self_contact_id_);

    auto join = std::bind(&server::__on_join, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);
    auto leave = std::bind(&server::__on_leave, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    auto receive = std::bind(&server::on_receive, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4,
        std::placeholders::_5, std::placeholders::_6);
    auto sended = std::bind(&server::on_sended, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    auto readed = std::bind(&server::on_readed, this,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    meth_["join"] = [join](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& map = *map_p;
        const std::string& id = mpark::get<std::string>(map.at("id").value);
        const std::string& sig = mpark::get<std::string>(map.at("sig").value);
        bool rep = mpark::get<bool>(map.at("rep").value);
        join(con, id, sig, rep);
        return true;
    };

    meth_["relay_join"] = [this](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        try
        {
            c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
            std::map<std::string, c2c::item_t>& map = *map_p;
            uint64_t hop = mpark::get<uint64_t>(map.at("hop").value);

            if(hop < chat::TTL)
            {
                const std::string& to = mpark::get<std::string>(map.at("peer").value);
                c2c::map_t& data = mpark::get<c2c::map_t>(map.at("data").value);
                if(!__do_send_to(to, "join", data))
                    __do_relay_to(to, hop+1, "relay_join", data);
            }
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
        return true;
    };

    meth_["leave"] = [leave](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& map = *map_p;
        const std::string& id = mpark::get<std::string>(map.at("id").value);
        const std::string& sig = mpark::get<std::string>(map.at("sig").value);
        leave(con, id, sig);
        return true;
    };

    meth_["relay_leave"] = [this](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        try
        {
            c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
            std::map<std::string, c2c::item_t>& map = *map_p;
            uint64_t hop = mpark::get<uint64_t>(map.at("hop").value);

            if(hop < chat::TTL)
            {
                const std::string& to = mpark::get<std::string>(map.at("peer").value);
                c2c::map_t& data = mpark::get<c2c::map_t>(map.at("data").value);
                if(!__do_send_to(to, "leave", data))
                    __do_relay_to(to, hop+1, "relay_leave", data);
            }
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
        return true;
    };

    meth_["send"] = [receive](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        try
        {
            c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
            std::map<std::string, c2c::item_t>& map = *map_p;
            const std::string& from = mpark::get<std::string>(map.at("from").value);
            const std::string& to = mpark::get<std::string>(map.at("to").value);
            const std::string& data = mpark::get<std::string>(map.at("data").value);
            int n_index = int(mpark::get<uint64_t>(map.at("n").value));
            time_t tm = time_t(mpark::get<uint64_t>(map.at("tm").value));
            receive(con, from, to, data, n_index, tm);
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
        return true;
    };

    meth_["relay_send"] = [this](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        try
        {
            c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
            std::map<std::string, c2c::item_t>& map = *map_p;
            uint64_t hop = mpark::get<uint64_t>(map.at("hop").value);

            if(hop < chat::TTL)
            {
                const std::string& to = mpark::get<std::string>(map.at("peer").value);
                c2c::map_t& data = mpark::get<c2c::map_t>(map.at("data").value);
                if(!__do_send_to(to, "send", data))
                    __do_relay_to(to, hop+1, "relay_send", data);
            }
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
        return true;
    };

    meth_["sended"] = [sended](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& map = *map_p;
        const std::string& to = mpark::get<std::string>(map.at("to").value);
        const std::string& from = mpark::get<std::string>(map.at("from").value);
        int n_index = int(mpark::get<uint64_t>(map.at("n").value));
        time_t tm = time_t(mpark::get<uint64_t>(map.at("tm").value));
        sended(con, to, from, n_index, tm);
        return true;
    };

    meth_["relay_sended"] = [this](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& rel_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& rel = *rel_p;
        uint64_t hop = mpark::get<uint64_t>(rel.at("hop").value);

        if(hop < chat::TTL)
        {
            const std::string& peer = mpark::get<std::string>(rel.at("peer").value);
            c2c::map_t& data = mpark::get<c2c::map_t>(rel.at("data").value);
            if(!__do_send_to(peer, "sended", data))
                __do_relay_to(peer, hop+1, "relay_sended", data);
        }
        return true;
    };

    meth_["readed"] = [readed](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& map_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& map = *map_p;
        const std::string& to = mpark::get<std::string>(map.at("to").value);
        int n_index = int(mpark::get<uint64_t>(map.at("n").value));
        time_t tm = time_t(mpark::get<uint64_t>(map.at("tm").value));
        readed(con, to, n_index, tm);
        return true;
    };

    meth_["relay_readed"] = [this](c2c::connection_ptr con, c2c::var_t pars) -> bool
    {
        c2c::map_t& rel_p = mpark::get<c2c::map_t>(pars);
        std::map<std::string, c2c::item_t>& rel = *rel_p;
        uint64_t hop = mpark::get<uint64_t>(rel.at("hop").value);

        if(hop < chat::TTL)
        {
            const std::string& peer = mpark::get<std::string>(rel.at("peer").value);
            c2c::map_t& data = mpark::get<c2c::map_t>(rel.at("data").value);
            if(!__do_send_to(peer, "readed", data))
                __do_relay_to(peer, hop+1, "relay_readed", data);
        }
        return true;
    };

    p2p_->join("chat",
        std::bind(&server::__open, this, _1),
        std::bind(&server::__close, this, _1),
        std::bind(&server::__call, this, _1, _2, _3),
        std::bind(&server::__call_r, this, _1, _2, _3, _4));
}

void server::start()
{
    std::vector<std::string> proto = {{"chat"}};
    for(auto s : u_seeds_)
        p2p_->connect(proto, s);

    tm_.expires_from_now(boost::posix_time::millisec(1000));
    tm_.async_wait(std::bind(&server::on_idle, shared_from_this(), std::placeholders::_1));
}

void server::stop()
{
    do_leave();
    tm_.cancel();
}

void server::on_idle(boost::system::error_code const& ec)
{
    if(!ec)
    {
        if(seeds_.size() < chat::SEED_CONNECTIONS && seeds_.size() < u_seeds_.size())
        {
            std::vector<std::string> proto = {{"chat"}};
            for(auto s : u_seeds_)
                p2p_->connect(proto, s);
        }

        if(idle_ == 3)
            do_join();

        idle_++;
    }
    else
    {
        LOG(ERROR) << ec.message();
    }

    tm_.expires_from_now(boost::posix_time::millisec(1000));
    tm_.async_wait(std::bind(&server::on_idle, shared_from_this(), std::placeholders::_1));
}

std::shared_ptr<c2c::p2p::server> server::p2p()
{
    return p2p_;
}

void server::__open(c2c::connection_ptr con)
{
    peers_.insert(con);
    for(auto s=u_seeds_.begin(); s<u_seeds_.end(); s++) {
        if(con->uri == *s) {
            seeds_.insert(con);
            //u_seeds_.erase(s);
            break;
        }
    }
    std::string id;
    id.assign((const char *)con->id.data(), con->id.size());
    on_join(id);
}

void server::__close(c2c::connection_ptr con)
{
    peers_.erase(con);
    seeds_.erase(con);

    on_leave(std::string((const char *)con->id.data(), int(con->id.size())));
}

bool server::__call(c2c::connection_ptr con, const std::string& method, const c2c::var_t& params)
{
    try
    {
        meth_map::iterator m = meth_.find(method);
        if(m != meth_.end())
            return m->second(con, params);
    }
    catch (const std::exception& e)
    {
        LOG(ERROR) << e.what();
    }
    return false;
}

bool server::__call_r(c2c::connection_ptr con, const std::string& method, const c2c::var_t& params, c2c::call_t reply)
{
    try
    {
        meth_r_map::iterator m = meth_r_.find(method);
        if(m != meth_r_.end())
            return m->second(con, params, reply);
    }
    catch (const std::exception& e)
    {
        LOG(ERROR) << e.what();
    }
    return false;
}


void server::__on_join(c2c::connection_ptr con, const std::string& id, const std::string& sig, bool rep)
{
    if(id.size() != crypto_sign_PUBLICKEYBYTES || sig.size() != crypto_sign_BYTES)
    {
        LOG(ERROR) << "error join id: " << c2c::bin2hex(id);
        return;
    }
    if(0 != crypto_sign_verify_detached(
        (uint8_t *)sig.data(), (uint8_t *)id.data(), id.size(), (uint8_t *)id.data()))
    {
        LOG(ERROR) << "error verify joined id : " << c2c::bin2hex(id);
        return;
    }

    on_join(id);

    if(rep)
    {
        std::string sg;
        p2p_->sign(self_contact_id_, sg);
        c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
        std::map<std::string, c2c::item_t>& map = *data;
        map["id"].value = self_contact_id_;
        map["sig"].value = sg;
        map["rep"].value = false;
        if(!__do_send_to(id, "join", data))
            __do_relay_to(id, 0, "relay_join", data);
    }
}

void server::__on_leave(c2c::connection_ptr con, const std::string& id, const std::string& sig)
{
    if(id.size() != crypto_sign_PUBLICKEYBYTES || sig.size() != crypto_sign_BYTES)
    {
        LOG(ERROR) << "error join id: " << c2c::bin2hex(id);
        return;
    }
    if(0 != crypto_sign_verify_detached(
        (uint8_t *)sig.data(), (uint8_t *)id.data(), id.size(), (uint8_t *)id.data()))
    {
        LOG(ERROR) << "error verify joined id : " << c2c::bin2hex(id);
        return;
    }
    on_leave(id);
}

void server::on_join(const std::string& id)
{
    c2c::cxx::db db = data_.at(pfx::CONTACT);
    std::string raw = db.get(id);
    if(raw.empty())
    {
        contact_info c;
        const char *color = random_color();
        c.ab_background.assign(color, 3);
        c.tm = time(nullptr);
        if(0.3*uint8_t(color[0])+0.59*uint8_t(color[1])+0.11*uint8_t(color[2]) > 128.0)
            c.ab_color.assign("\x00\x00\x00", 3);
        else
            c.ab_color.assign("\xFF\xFF\xFF", 3);
        c.type = contact_info::USER;
        db.put(id, c);
    }

    emit on_contact_join(id);

    time_t now = time(nullptr);
    c2c::cxx::db mdb = data_.at(pfx::MESSAGE_UNSEND, id);

    c2c::cxx::db mdb_store(mdb);
    mdb_store.begin();

    if(mdb.first())
    {
        for(bool b=true; b; b=mdb.next())
        {
            time_t t; size_t off = 0;
            c2c::cxx::result<time_t>(t).on(mdb.val(), off);

            if(t > now) continue;

            std::string to; int n; off = 0;
            c2c::cxx::result<std::string, int>(to, n).on(mdb.key(), off);

            message_info m; off = 0;
            c2c::cxx::result<message_info>(m).on(data_.get(pfx::MESSAGE, to, n, self_contact_id_), off);

            m.to = to;

            if( !__do_send_to(id, m) )
                __do_relay_to(id, 0, m);

            mdb_store.put(to, n, now + 60);
        }
        mdb_store.commit();
    }
}

void server::on_leave(const std::string& id)
{
    emit on_contact_leave(id);
}

void server::do_join()
{

    std::string sig;
    p2p_->sign(self_contact_id_, sig);

    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["id"].value = self_contact_id_;
    map["sig"].value = sig;
    map["rep"].value = true;

    std::set<std::string> list;

    c2c::cxx::db db = data_.at(pfx::CONTACT);
    for(bool r=db.first(); r; r=db.next())
    {
        std::string p; contact_info c; size_t off = 0;
        off = 0; c2c::cxx::result<std::string>(p).on(db.key(), off);
        off = 0; c2c::cxx::result<contact_info>(c).on(db.val(), off);
        if(c.type == contact_info::GROUP)
        {
            c2c::cxx::db gdb = data_.at(pfx::GROUP_PEERS, p);
            if(gdb.first())
            {
                for(bool b=true; b; b=gdb.next())
                {
                    std::string gp; size_t off = 0;
                    c2c::cxx::result<std::string>(gp).on(gdb.key(), off);

                    if(gp ==  self_contact_id_)
                        continue;

                    list.insert(gp);
                }
            }
        }
        else
            list.insert(p);
    }

    for(auto n : list)
    {
        if(!__do_send_to(n, "join", data))
            __do_relay_to(n, 0, "relay_join", data);
    }
}

void server::do_leave()
{
    std::string sig;
    p2p_->sign(self_contact_id_, sig);

    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["id"].value = self_contact_id_;
    map["sig"].value = sig;

    std::set<std::string> list;

    c2c::cxx::db db = data_.at(pfx::CONTACT);
    for(bool r=db.first(); r; r=db.next())
    {
        std::string p; contact_info c; size_t off = 0;
        off = 0; c2c::cxx::result<std::string>(p).on(db.key(), off);
        off = 0; c2c::cxx::result<contact_info>(c).on(db.val(), off);
        if(c.type == contact_info::GROUP)
        {
            c2c::cxx::db gdb = data_.at(pfx::GROUP_PEERS, p);
            if(gdb.first())
            {
                for(bool b=true; b; b=gdb.next())
                {
                    std::string gp; size_t off = 0;
                    c2c::cxx::result<std::string>(gp).on(gdb.key(), off);

                    if(gp ==  self_contact_id_)
                        continue;

                    list.insert(gp);
                }
            }
        }
        else
            list.insert(p);
    }

    for(auto n : list)
    {
        if(!__do_send_to(n, "leave", data))
            __do_relay_to(n, 0, "relay_leave", data);
    }
}

void server::on_receive(
        c2c::connection_ptr con,
        const std::string& from,
        const std::string& to,
        const std::string& message,
        int n_index,
        time_t tm)
{
    message_info m;
    m.m = message;
    m.n = n_index;
    m.t.c = tm;
    m.t.s = time(nullptr);
    m.t.r = 0;
    int cnt = 0; size_t off = 0;
    const std::string& target = (to == self_contact_id_ ? from : to);
    c2c::cxx::result<int>(cnt).on(data_.get(pfx::MESSAGE_CNT, target),off);
    data_.begin();
    data_.put(pfx::MESSAGE, target, cnt, from, m);
    data_.put(pfx::MESSAGE_CNT, target, cnt+1);
    data_.commit();
    emit on_message_received_from(target, cnt, from);
    do_sended(from, to, n_index, m.t.s);
    emit on_contact_change(target);
}

bool server::__do_send_to(const std::string& peer, const message_info& m) const
{
    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["from"].value = self_contact_id_;
    map["to"].value = m.to;
    map["data"].value = m.m;
    map["n"].value = m.n;
    map["tm"].value = m.t.c;
    return __do_send_to(peer, "send", data);
}

void server::do_sended(const std::string& peer, const std::string& to, int n_index, time_t tm) const
{
    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["to"].value = to;
    map["from"].value = self_contact_id_;
    map["n"].value = n_index;
    map["tm"].value = uint64_t(tm);
    if(!__do_send_to(peer, "sended", data))
        __do_relay_to(peer, 0, "relay_sended", data);
}

void server::on_sended(c2c::connection_ptr con, const std::string& to, const std::string& from, int n_index, time_t tm)
{
    message_info m; size_t off = 0;
    c2c::cxx::result<message_info>(m).on(data_.get(pfx::MESSAGE, to, n_index, self_contact_id_), off);
    m.t.s = tm;
    data_.put(pfx::MESSAGE, to, n_index, self_contact_id_, m);
    data_.del(pfx::MESSAGE_UNSEND, from, to, n_index);
    emit on_message_changed(to, n_index);
}

void server::do_readed(const std::string& peer, const std::string& to, int n_index, time_t tm) const
{
    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["to"].value = to;
    map["n"].value = n_index;
    map["tm"].value = uint64_t(tm);
    if(!__do_send_to(peer, "readed", data))
        __do_relay_to(peer, 0, "relay_readed", data);
}

void server::on_readed(c2c::connection_ptr con, const std::string& to, int n_index, time_t tm)
{
    message_info m; size_t off = 0;
    c2c::cxx::result<message_info>(m).on(data_.get(pfx::MESSAGE, to, n_index, self_contact_id_), off);
    m.t.r = tm;
    data_.put(pfx::MESSAGE, to, n_index, self_contact_id_, m);
    emit on_message_changed(to, n_index);
}

bool server::__do_send_to(const std::string& peer, const std::string& cmd, const c2c::map_t& data) const
{
    for(auto n=peers_.begin(); n!=peers_.end(); n++)
    {
        c2c::connection_ptr con = *n;
        std::string peer_id;
        peer_id.assign((const char *)con->id.data(), size_t(con->id.size()));
        if(peer_id == peer)
        {
            std::vector<c2c::connection_ptr> peers = { con };
            p2p_->call(cmd, data, peers);
            return true;
        }
    }
    return false;
}

void server::__do_relay_to(const std::string& peer, uint64_t hop, const message_info& m) const
{
    c2c::map_t data = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& map = *data;
    map["from"].value = self_contact_id_;
    map["to"].value = m.to;
    map["data"].value = m.m;
    map["n"].value = m.n;
    map["tm"].value = m.t.c;
    __do_relay_to(peer, hop, "relay_send", data);
}

void server::__do_relay_to(const std::string& peer, uint64_t hop, const std::string& cmd, const c2c::map_t& data) const
{
    c2c::map_t rel_p = std::make_shared<std::map<std::string, c2c::item_t>>();
    std::map<std::string, c2c::item_t>& rel = *rel_p;
    rel["peer"].value = peer;
    rel["hop"].value = hop;
    rel["data"].value = data;
    std::vector<c2c::connection_ptr> peers;
    for(auto s : seeds_) {
        std::string id((const char *)s->id.data(), s->id.size());
        peers.push_back(s);
    }
    p2p_->call(cmd, rel_p, peers);
}

void server::do_send_to(std::string to, int n_index)
{
    message_info m; size_t off = 0;
    c2c::cxx::result<message_info>(m).on(data_.get(pfx::MESSAGE, to, n_index, self_contact_id_), off);

    m.to = to;

    c2c::cxx::db gdb = data_.at(pfx::GROUP_PEERS, to);
    if(gdb.first())
    {
        for(bool b=true; b; b=gdb.next())
        {
            std::string peer; size_t off = 0;
            c2c::cxx::result<std::string>(peer).on(gdb.key(), off);

            if(peer ==  self_contact_id_)
                continue;

            // send to one peer of group 'to'
            if( !__do_send_to(peer, m) )
                __do_relay_to(peer, 0 /* first hop =0 */, m);

            data_.put(pfx::MESSAGE_UNSEND, peer, to, n_index, m.t.c);
        }
    }
    else
    {
        // send to peer 'to'
        if( !__do_send_to(to, m) )
            __do_relay_to(to, 0, m);

        data_.put(pfx::MESSAGE_UNSEND, to, to, n_index, m.t.c);
    }
}

}
