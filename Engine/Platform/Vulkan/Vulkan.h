#pragma once

#include "EngineMacro.h"
#include <vulkan/vulkan.hpp>

#define VK_CHECK(res, expect, ...) \
{ \
    bool bVK_CHECK_RESULT_VALUE = static_cast<vk::Result>(res) == expect; \
    VT_CORE_ASSERT( \
        bVK_CHECK_RESULT_VALUE, \
        "Vulkan check failed at ",      \
        __FILE__,\
        __LINE__,\
        " : ",\
        __VA_ARGS__,\
        " {}",\
        vk::to_string(static_cast<vk::Result>(res)));\
}
