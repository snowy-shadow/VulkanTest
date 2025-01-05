module;
#include <string>
#include <vector>
#include <VT_Export>
export module VT.PipelineManager;
import VT.RendererStructure;
import VT.RendererEnum;

export namespace VT
{

// Use render graph for automatic resource management
class VT_ENGINE_EXPORT ResourceManager
{
public:
    virtual bool CreateDescriptorLayout(const std::vector<VT::DescriptorLayoutBindingInfo>& CreateInfo,
                                        std::string Name)                                       = 0;
    virtual bool RemoveDescriptorLayout(const std::string& Name)                                = 0;
    virtual bool CreateDescriptorPool(const VT::DescriptorPoolInfo& PoolInfo, const char* Name) = 0;
    virtual bool Allocate(const VT::DescriptorAllocateInfo&, const char* Name)                  = 0;

    virtual bool CreatePipelineLayout(std::vector<std::string> DescriptorLayouts,
                                      std::vector<PushConstantInfo> PushConstants,
                                      std::string Name)        = 0;
    virtual bool RemovePipelineLayout(const std::string& Name) = 0;

    virtual ~ResourceManager() = default;
};

class VT_ENGINE_EXPORT PipelineManager
{
public:
    virtual bool CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& CreateInfo, std::string Name) = 0;
    virtual bool RemovePipeline(const std::string& Name)                                                = 0;

    virtual bool CreateRenderPass(RenderPassCreateInfo CreateInfo, std::string Name) = 0;
    virtual bool RemoveRenderPass(const std::string& Name)                           = 0;

    virtual ResourceManager& ResourceManager() = 0;

    virtual ~PipelineManager() = default;
};
} // namespace VT