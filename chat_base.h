#ifndef CHAT_BASE_H
#define CHAT_BASE_H

#include <string>

namespace pfx
{
    size_t const SELF_CONTACT_ID = 1;
    size_t const CONTACT = 10;
    size_t const GROUP_PEERS = 12;
    size_t const MESSAGE = 20;
    size_t const MESSAGE_CNT = 21;
    size_t const MESSAGE_READED = 22;
    size_t const MESSAGE_UNSEND = 23;
}

namespace chat {
    size_t const TTL = 3;
    size_t const SEED_CONNECTIONS = 8;
}

#endif
