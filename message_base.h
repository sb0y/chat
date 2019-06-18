#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <msgpack.hpp>

struct message_info {
    std::string m, to;
    int n;
    struct {
        time_t  c, // created
                s, // sended
                r; // readed
        MSGPACK_DEFINE_MAP(c, s, r)
    } t;
    MSGPACK_DEFINE_MAP(m, n, t)
};

#endif
