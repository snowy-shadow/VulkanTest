module;
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.PipelineManager;
import VT.PipelineManager;
import VT.RendererStructure;
import VT.RendererEnum;

export namespace VT::Vulkan
{
class PipelineManager;

struct Pipeline
{
    vk::Pipeline m_Pipeline = VK_NULL_HANDLE;

    explicit Pipeline(const vk::Pipeline pipeline) : m_Pipeline(pipeline) {}
};

class ResourceManager final : public VT::ResourceManager
{
public:
    virtual bool CreateDescriptorLayout(const std::vector<VT::DescriptorLayoutBindingInfo>& CreateInfo,
                                        std::string Name) override;
    virtual bool RemoveDescriptorLayout(const std::string& Name) override;

    virtual bool CreateDescriptorPool(const VT::DescriptorPoolInfo& PoolInfo, const char* Name) override;
    virtual bool Allocate(const VT::DescriptorAllocateInfo&, const char* Name) override;

    virtual bool CreatePipelineLayout(std::vector<std::string> DescriptorLayouts,
                                      std::vector<PushConstantInfo> PushConstants,
                                      std::string Name) override;
    virtual bool RemovePipelineLayout(const std::string& Name) override;

    ResourceManager(vk::Device);
    ~ResourceManager();

private:
    std::unordered_map<std::string, vk::PipelineLayout> m_PipelineLayout;
    std::unordered_map<const char*, vk::DescriptorPool> m_DescriptorPool;
    std::unordered_map<const char*, std::vector<vk::DescriptorSet>> m_DescriptorSet;
    std::unordered_map<std::string, vk::DescriptorSetLayout> m_DescriptorLayout;

    vk::Device m_LogicalDevice = VK_NULL_HANDLE;

    friend class Vulkan::PipelineManager;
};

class PipelineManager final : public VT::PipelineManager
{
public:
    virtual bool CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& CreateInfo, std::string Name) override;
    virtual bool RemovePipeline(const std::string& Name) override;

    virtual bool CreateRenderPass(RenderPassCreateInfo CreateInfo, std::string Name) override;
    virtual bool RemoveRenderPass(const std::string& Name) override;

    virtual Vulkan::ResourceManager& ResourceManager() override;

    PipelineManager(vk::Device);
    ~PipelineManager();

private:
    // switch to vector. Key is index
    std::unordered_map<std::string, Vulkan::Pipeline> m_Pipeline;
    std::unordered_map<std::string, vk::RenderPass> m_RenderPass;
    Vulkan::ResourceManager m_ResourceManager;
    vk::Device m_LogicalDevice = VK_NULL_HANDLE;
};

} // namespace VT::Vulkan
