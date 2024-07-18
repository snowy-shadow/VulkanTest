module;
#include "VT_Export"

#include <cstdint>
#include <glm/glm.hpp>
export module VT.RendererType;

import VT.Texture;

export namespace VT
{
using ID                               = uint32_t;
static constexpr ID InvalidID          = -1;
static constexpr uint32_t InvalidIndex = -1;
}

export namespace VT::RendererType
{
enum class VT_ENGINE_EXPORT API
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
    Texture* pTextures;
};

} // namespace VT::RendererType
