#ifndef CONTACT_BASE_H
#define CONTACT_BASE_H

#include <msgpack.hpp>

struct contact_info
{
    enum TYPE { USER, GROUP };
    MSGPACK_DEFINE_MAP(name, alias, ab_color, ab_background, tm, type)
    std::string name, alias, ab_color, ab_background; time_t tm; size_t type;
};

#endif
