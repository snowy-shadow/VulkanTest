module;
#include "VT_Export"

#include <cstdint>
#include <glm/glm.hpp>
export module VT.RendererType;

import VT.Texture;

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
    Texture** pTexture;
};

} // namespace VT
