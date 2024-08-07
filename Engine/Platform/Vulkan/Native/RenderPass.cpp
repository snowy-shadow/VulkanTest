module;
#include "EngineMacro.h"
#include "Platform/Vulkan/Vulkan.h"
module VT.Platform.Vulkan.Native.RenderPass;

import VT.Log;

namespace VT::Vulkan::Native
{
RenderPass::RenderPass(vk::Device LogicalDevice) : m_LogicalDevice(LogicalDevice) {}

void RenderPass::Init(vk::Device LogicalDevice) { m_LogicalDevice = LogicalDevice; }

void RenderPass::Create(
    const std::vector<vk::AttachmentDescription>& Attachment,
    const std::vector<vk::SubpassDescription>& Subpass,
    const std::vector<vk::SubpassDependency>& SubpassDependency)
{
    vk::RenderPassCreateInfo RenderPassInfo {
        .attachmentCount = static_cast<uint32_t>(Attachment.size()),
        .pAttachments    = Attachment.data(),
        .subpassCount    = static_cast<uint32_t>(Subpass.size()),
        .pSubpasses      = Subpass.data(),
        .dependencyCount = static_cast<uint32_t>(SubpassDependency.size()),
        .pDependencies   = SubpassDependency.data()};

    vk::Result Result;

    std::tie(Result, m_RenderPass) = m_LogicalDevice.createRenderPass(RenderPassInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create renderpass");
}

vk::CommandBuffer RenderPass::Begin(
    vk::CommandBuffer Buffer,
    vk::Framebuffer FrameBuffer,
    vk::Rect2D RenderArea,
    const std::vector<vk::ClearValue>& ClearValue) const
{
    VT_CORE_ASSERT(m_RenderPass != VK_NULL_HANDLE, "Renderpass not created! Cannot begin renderpass");

    vk::RenderPassBeginInfo RenderPassBeginInfo {
        .renderPass      = m_RenderPass,
        .framebuffer     = FrameBuffer,
        .renderArea      = RenderArea,
        .clearValueCount = static_cast<uint32_t>(ClearValue.size()),
        .pClearValues    = ClearValue.data()};
    Buffer.beginRenderPass(RenderPassBeginInfo, vk::SubpassContents::eInline);

    return Buffer;
}

vk::CommandBuffer RenderPass::End(vk::CommandBuffer Buffer)
{
    Buffer.endRenderPass();
    return Buffer;
}

vk::RenderPass RenderPass::Get() const { return m_RenderPass; }

RenderPass::~RenderPass()
{
    if (m_RenderPass != VK_NULL_HANDLE)
    {
        m_LogicalDevice.destroyRenderPass(m_RenderPass);
    }
}

} // namespace VT::Vulkan::Native