#pragma once

#include "EngineMacro.h"
#include <vulkan/vulkan.hpp>

#define VK_CHECK(res, expect, ...)                        \
    {                                                     \
        VT_CORE_ASSERT(                                   \
        (static_cast<vk::Result>(res) == expect), \
        "Vulkan check failed at ",      \
        __FILE__,\
        __LINE__,\
        " : ",\
        __VA_ARGS__,\
        " {}",\
        vk::to_string(static_cast<vk::Result>(res)));\
}
