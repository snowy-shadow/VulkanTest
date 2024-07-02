module;
#include <vulkan/vulkan.hpp>
export module VT.Platform.Vulkan.Native.RenderPass;

export namespace VT::Vulkan::Native
{
class RenderPass
{
public:
    RenderPass(vk::Device LogicalDevice = VK_NULL_HANDLE);

    void Init(vk::Device LogicalDevice);

    void Create(
        const std::vector<vk::AttachmentDescription>& Attachment,
        const std::vector<vk::SubpassDescription>& Subpass,
        const std::vector<vk::SubpassDependency>& SubpassDependency);

    vk::CommandBuffer Begin(
        vk::CommandBuffer Buffer,
        vk::Framebuffer FrameBuffer,
        vk::Rect2D RenderArea,
        const std::vector<vk::ClearValue>& ClearValue) const;

    vk::CommandBuffer End(vk::CommandBuffer Buffer);
    ~RenderPass();

private:
    vk::Device m_LogicalDevice;
    vk::RenderPass m_RenderPass;
};
} // namespace VT::Vulkan::Native
