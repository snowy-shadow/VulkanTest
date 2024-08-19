module;
#include "VT_Export"
#include <cstdint>
#include <cstddef>
export module VT.Texture;

export namespace VT
{
struct VT_ENGINE_EXPORT TextureCreateInfo
{
    const char* File = 0;

    uint32_t Width    = 0;
    uint32_t Height   = 0;
    uint32_t Channels = 0;
    bool Transluscent = false;
    uint32_t DataSize = 0;
     std::byte* pData  = nullptr;
};

class VT_ENGINE_EXPORT Texture
{
public:
    uint32_t Width;
    uint32_t Height;
    uint32_t Channels;
    bool Transluscent;

    virtual ~Texture() = default;
};
} // namespace VT
