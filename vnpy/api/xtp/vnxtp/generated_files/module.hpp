#pragma once

#include <autocxxpy/autocxxpy.hpp>

struct tag_vnxtp{};
struct module_vnxtp{
    static autocxxpy::cross_assign cross;
    static autocxxpy::object_store objects;
    static inline void process_post_assign()
    {
        cross.process_assign(objects);
        cross.clear();
        objects.clear();
    }
};
using module_tag=tag_vnxtp;


