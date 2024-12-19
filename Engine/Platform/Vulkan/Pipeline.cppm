module;
#include <vulkan/vulkan.hpp>
#include "VT_Export"

export module VT.Platform.Vulkan.Pipeline;

import VT.Pipeline;
import VT.RendererStructure;

export namespace VT::Vulkan
{
    class Pipeline : VT::Pipeline
    {
    public:
        virtual void Bind() override {}
    };

    // FIX : VT::GraphicsPipeline not defined
    class GraphicsPipeline : Pipeline
    {
    public:
        void Create(GraphicsPipelineCreateInfo);

        // void Destroy();

        virtual void Bind() override {};

        vk::Pipeline Pipline = VK_NULL_HANDLE;
        vk::Device LogicalDevice = VK_NULL_HANDLE;
    };
} // namespace VT::Vulkan
