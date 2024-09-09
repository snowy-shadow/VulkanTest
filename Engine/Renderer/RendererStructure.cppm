module;
#include "VT_Export"
#include <cstdint>
#include <glm/glm.hpp>
export module VT.RendererStructure;

import VT.Texture;
import VT.Util.DataStructure;


export namespace VT
{
    using ID = uint32_t;
    constexpr ID InvalidID = (ID)-1;
    constexpr uint32_t InvalidIndex = (ID)-1;
    constexpr uint32_t TextureInvalideID = (ID)-1;

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

    struct VT_ENGINE_EXPORT DescriptorLayout
    {
        uint32_t Binding;
        DescriptorType DescriptorType;
        uint32_t DescriptorCount;
        ShaderStageFlag Stage;
        uint32_t PushConstantSize = 0;

        // handle to native api layout obj. nullptr if such doesn't exist
        virtual void* GetHandle() = 0;
    };

    struct VT_ENGINE_EXPORT ShaderSpv
    {
        std::vector<uint32_t> Spv;
        DescriptorLayout& Layout;
    };

}