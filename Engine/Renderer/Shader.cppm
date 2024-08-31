module;
#include "VT_Export"

#include <unordered_map>
#include <cstddef>
#include <array>

export module VT.Shader;

export namespace VT
{
struct VT_ENGINE_EXPORT ShaderInitInfo
{
    uint32_t MaxShader;
    uint32_t MaxUniformBuffer;
    uint32_t MaxGlobalTexture;
    uint32_t MaxInstanceTexture;
};

class VT_ENGINE_EXPORT Shader
{
public:
    Shader(ShaderInitInfo);

    ~Shader();

private:
};

// MOVE TO VULKAN FOLDER
namespace Vulkan
{
class Shader : public VT::Shader
{
public:
    Shader(ShaderInitInfo);


private:
    std::array<std::byte, 128> m_PushConstant;
};
} // namespace Vulkan

} // namespace VT