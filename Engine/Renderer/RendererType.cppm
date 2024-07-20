module;
#include "VT_Export"

#include <cstdint>
#include <glm/glm.hpp>
export module VT.RendererType;

export namespace VT
{
using ID                             = uint32_t;
constexpr ID InvalidID               = (ID) -1;
constexpr uint32_t InvalidIndex      = (ID) -1;
constexpr uint32_t TextureInvalideID = (ID) -1;

enum class VT_ENGINE_EXPORT GraphicsAPI
{
    eNone,
    eVulkan
};

struct VT_ENGINE_EXPORT TextureCreateInfo
{
    ID ID;
    uint32_t Width;
    uint32_t Height;
    uint32_t ChannelCount;
    uint32_t Generation;
    uint32_t DataSize;
    void* pData;
    bool Transparent;
};

class VT_ENGINE_EXPORT Texture
{
public:
    ID ID;
    uint32_t Width;
    uint32_t Height;
    uint32_t ChannelCount;
    uint32_t Generation;
    bool Transparent;

    virtual ~Texture() = default;
};

struct VT_ENGINE_EXPORT RenderPacket
{
};

struct VT_ENGINE_EXPORT UniformCameraData
{
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
};

struct VT_ENGINE_EXPORT UniformObjectData
{
    glm::vec4 DiffuseColor;
};

struct VT_ENGINE_EXPORT GeometryRenderData
{
    ID ID;
    glm::mat4 Model;
    uint32_t TextureCount;
    Texture* pTextures;
};

} // namespace VT
