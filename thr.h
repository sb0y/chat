#ifndef L_SRV_H
#define L_SRV_H

#include "c2c/var.h"
#include "c2c/func.h"

#include <memory>
#include <thread>

#include <boost/asio/io_service.hpp>

namespace c2c { namespace p2p { class server; } }
namespace chat { class server; }

class thr : public std::enable_shared_from_this<thr>
{
    std::shared_ptr<c2c::p2p::server> p2p_;
    std::shared_ptr<chat::server> chat_;

    std::thread thr_;
    boost::asio::io_service ios_;

public:
    thr(int arvc, char *argv[]);

    void start();
    void stop();

    boost::asio::io_service& get_io_service() { return ios_; }
    std::shared_ptr<chat::server> chat();
};

#endif
