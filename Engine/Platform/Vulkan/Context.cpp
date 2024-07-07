module;
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

module VT.Platform.Vulkan.Context;

import VT.Log;
import VT.ShaderCompiler;
import VT.OrthographicCamera;

namespace VT::Vulkan
{
Context::Context(Shared<Window> Window) : m_Window(Window), m_Camera(new OrthographicCamera) {}

bool Context::BeginFrame()
{
	vk::Device LogicalDevice = m_LogicalDevice.Get();

    /* =============================================================================================================
     *									Setup
     * =============================================================================================================
     */
	VT_CORE_ASSERT(m_DrawFence.Wait(), "Failed to wait for draw fence");

    if (m_ScheduleResize)
    {
        Resize(m_Window->GetWidth(), m_Window->GetHeight());
        m_ScheduleResize = false;
    }
    // Swapchain Image index
    auto [bResult, ImageIndex] = m_Swapchain.AcquireNextImage(m_ImageAvailable);
    VT_CORE_ASSERT(bResult, "Failed to obtain swapchain image");

    vk::CommandBuffer& CmdBuffer = m_DrawBuffer[m_CurrentFrameCount];

    VK_CHECK(
        CmdBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources),
        vk::Result::eSuccess,
        "Failed to reset command buffer");

    VK_CHECK(
        CmdBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit }),
        vk::Result::eSuccess,
        "Failed to begin cmd buffer");

    // Renderpass
    {
        // dynamic states
        const auto ImageExtent = m_Swapchain.GetInfo().imageExtent;
        vk::Viewport ViewPort{ .x = 0.f,
                               .y = 0.f,
                               .width = static_cast<float>(ImageExtent.width),
                               .height = static_cast<float>(ImageExtent.height),
                               .minDepth = 0.f,
                               .maxDepth = 1.f };
        vk::Rect2D Scissor{
            .offset = { 0, 0 },
            .extent = ImageExtent
        };

        CmdBuffer.setViewport(0, ViewPort);
        CmdBuffer.setScissor(0, Scissor);

        std::vector<vk::ClearValue> ClearColor
        {
            { .color = { { { 0.0f, 0.3f, 0.6f, 1.0f } } }
            },

            {
                .depthStencil = { 1.0f, 0 }
            }
        };

        (void)m_RenderPass.Begin(CmdBuffer, m_FrameBuffer[ImageIndex].Get(), Scissor, ClearColor);
    }

    m_TriangleShader.Bind(CmdBuffer, vk::PipelineBindPoint::eGraphics);
    if (bUpdateCameraTransform)
    {
        m_TriangleShader.UpdateCameraTransform(CmdBuffer, m_Camera->GetTransform());
        bUpdateCameraTransform = false;
        VT_CORE_TRACE("Camera transform updated");
    }
    vk::Buffer VertexBuffer = m_VertexBuffer.Get();
    vk::DeviceSize VertexBufferOffsetSize[] {0};
    CmdBuffer.bindVertexBuffers(0, 1, &VertexBuffer, VertexBufferOffsetSize);
    CmdBuffer.bindIndexBuffer(m_IndexBuffer.Get(), 0, vk::IndexType::eUint32);

    // Draw
    CmdBuffer.drawIndexed(6, 1, 0, 0, 0);

    return true;
}

bool Context::EndFrame()
{
    vk::CommandBuffer& CmdBuffer = m_DrawBuffer[m_CurrentFrameCount];

    m_RenderPass.End(CmdBuffer);

    VK_CHECK(CmdBuffer.end(), vk::Result::eSuccess, "Failed to end command buffer");

    VT_CORE_ASSERT(m_DrawFence.Reset(), "Failed to reset Draw fence");

    // Queue submit
    {

        vk::PipelineStageFlags RenderWaitMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        vk::SubmitInfo RenderSubmit{ .waitSemaphoreCount = 1,
                                     .pWaitSemaphores = &m_ImageAvailable,
                                     .pWaitDstStageMask = &RenderWaitMask,
                                     .commandBufferCount = 1,
                                     .pCommandBuffers = &CmdBuffer,
                                     .signalSemaphoreCount = 1,
                                     .pSignalSemaphores = &m_RenderFinished };

        VK_CHECK(
            m_GraphicQ.submit(RenderSubmit, m_DrawFence.Get()),
            vk::Result::eSuccess,
            "Failed to submit to graphic queue");
        // end Queue submit
    }

    {
        // Present Image
        const auto Swapchain       = m_Swapchain.Get();
        uint32_t CurrentImageIndex = m_Swapchain.GetCurrentImageIndex();

        vk::PresentInfoKHR PresentInfo{ .waitSemaphoreCount = 1,
                                        .pWaitSemaphores = &m_RenderFinished,
                                        .swapchainCount = 1,
                                        .pSwapchains = &Swapchain,
                                        .pImageIndices = &CurrentImageIndex };

        vk::Result Result = m_PresentQ.presentKHR(PresentInfo);

        switch (Result)
        {
            case vk::Result::eSuccess :
                break;
            case vk::Result::eSuboptimalKHR :
                // change window size
                m_ScheduleResize = true;
                break;

            default :
                VK_CHECK(Result, vk::Result::eSuccess, "Present error");
        }
    }

    m_CurrentFrameCount = (m_CurrentFrameCount + 1) % m_MaxFrameCount;

    return true;
}

void Context::OnEvent(Event& Event)
{
    switch (Event.GetEventType())
    {
        case EventType::eWindowResize :
        {
            const auto Dimension = static_cast<WindowResizeEvent&>(Event).GetDimensionXY();

            Resize(Dimension[0], Dimension[1]);
            break;
        }

        case EventType::eKeyPress:
        {
            bUpdateCameraTransform = true;
        }
        break;
        case EventType::eMouseMove:
        {
            bUpdateCameraTransform = true;
        }
        break;

        default :
            break;
    }
}

void Context::Init()
{
	VT_CORE_TRACE("Vulkan context init");
	/* ===============================================
	 *          Create Vulkan Instance
	 * ===============================================
	 */
	{
		std::vector<const char*> InstanceExtension;
		std::vector<const char*> InstanceLayer;

		switch (m_Window->GetWindowAPI())
		{
			case WindowAPI::eGLFWwindow :
			{
				VT_CORE_ASSERT(glfwVulkanSupported(), "GLFW Window context does not support vulkan");
				uint32_t GlfwExtensionCount;
				const char** GlfwExtension = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

				// append to extension list
				InstanceExtension.reserve(GlfwExtensionCount);
				InstanceExtension.insert(InstanceExtension.cend(), GlfwExtension, GlfwExtension + GlfwExtensionCount);

				break;
			}
		}

		m_Instance.Init({ .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) }, InstanceExtension, InstanceLayer);
	}

	VT_CORE_TRACE("Vulkan Instance created");
	/* ===============================================
	 *          Variables
	 * ===============================================
	 */
	std::vector<const char*> DeviceExtension{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	vk::Instance VulkanInstance = m_Instance.Get();
	/* ===============================================
	 *          Create Physical device
	 * ===============================================
	 */
	{
		const auto [Result, PDs] = VulkanInstance.enumeratePhysicalDevices();
		VK_CHECK(Result, vk::Result::eSuccess, "Could not enumerate physical devices");

		std::vector<vk::PhysicalDeviceProperties> PDProperties;
		PDProperties.reserve(PDs.size());

		for (const auto& PD : PDs)
		{
			PDProperties.emplace_back(PD.getProperties());
		}

		VT_CORE_ASSERT(
			m_PhysicalDevice.FindPhysicalDevice(PDs, PDProperties, DeviceExtension),
			"Cannot find compatible physical device");
	}

	VT_CORE_TRACE("Vulkan Physical Device created");
	/* ===============================================
	 *          Create Window Surface
	 * ===============================================
	 */
	switch (m_Window->GetWindowAPI())
	{
		case WindowAPI::eGLFWwindow :
		{
			VkSurfaceKHR Surface;

			VkResult Result = glfwCreateWindowSurface(
				VulkanInstance,
				static_cast<GLFWwindow*>(m_Window->GetNativeWindow()),
				nullptr,
				&Surface);
			VK_CHECK(Result, vk::Result::eSuccess, "Failed to create window surface");

			m_Surface.Init(VulkanInstance, Surface);
			break;
		}
	}

	VT_CORE_TRACE("Vulkan Window Surface created");
	/* ===============================================
	 *          Create Logical device
	 * ===============================================
	 */
	{
		// add queue
		// m_PhysicalDevice.addQueue(vk::QueueFlagBits::eGraphics, 1.f);
		// m_PhysicalDevice.addQueue(vk::QueueFlagBits::eTransfer, 1.f);
		std::vector Priority{ 0.f };
		VT_CORE_ASSERT(
			m_PhysicalDevice.FindGraphicsQueueWithPresent(m_Surface.Get(), Priority),
			"Failed to find graphics queue that can present");

		if (m_PhysicalDevice.SupportsPortabilitySubset())
		{
			DeviceExtension.emplace_back("VK_KHR_portability_subset");
		}

		// create device;
		m_LogicalDevice.Init(m_PhysicalDevice.CreateLogicalDevice(DeviceExtension));
	}

	VT_CORE_TRACE("Vulkan Logical Device created");
	/* ===============================================
	 *          Variables
	 * ===============================================
	 */
	vk::Device LogicalDevice = m_LogicalDevice.Get();

	/* ===============================================
	 *          Synchronization Obj
	 * ===============================================
	 */
	{
		vk::Result Result;
		std::tie(Result, m_ImageAvailable) = LogicalDevice.createSemaphore({});
		VK_CHECK(Result, vk::Result::eSuccess, "Failed to create image available semaphore");

		std::tie(Result, m_RenderFinished) = LogicalDevice.createSemaphore({});
		VK_CHECK(Result, vk::Result::eSuccess, "Failed to create render finish semaphore");

		m_DrawFence.Create(LogicalDevice, vk::FenceCreateFlagBits::eSignaled);
	}
	VT_CORE_TRACE("Vulkan Synchronization objs created");

	/* ===============================================
	 *          Swapchain
	 * ===============================================
	 */
	{
		m_Swapchain.Init(*m_Window, LogicalDevice, m_PhysicalDevice, m_Surface.Get(), 2);
		m_MaxFrameCount = m_Swapchain.GetMaxFrameCount();
	}
	/* ===============================================
	 *          Get Graphics and present queues
	 * ===============================================
	 */
	{
		uint32_t GraphicsQueueFamilyIndex, GraphicsQueueIndex, PresentQueueFamilyIndex, PresentQueueIndex;

		if (m_PhysicalDevice.GraphicsQueueCanPresent())
		{
			const auto DeviceQueueInfo = m_PhysicalDevice.GetGraphicsQueue();
			GraphicsQueueFamilyIndex   = DeviceQueueInfo.queueFamilyIndex;
			// only created 1 queue, so index 0
			GraphicsQueueIndex = 0;

			PresentQueueFamilyIndex = GraphicsQueueFamilyIndex;
			PresentQueueIndex       = GraphicsQueueIndex;
		}
		else
		{
			auto DeviceQueueInfo     = m_PhysicalDevice.GetGraphicsQueue();
			GraphicsQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
			// use the first queue
			GraphicsQueueIndex = 0;

			DeviceQueueInfo         = m_PhysicalDevice.GetPresentQueue();
			PresentQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
			// use the first queue
			PresentQueueIndex = 0;
		}

		m_GraphicQ = LogicalDevice.getQueue(GraphicsQueueFamilyIndex, GraphicsQueueIndex);
		m_PresentQ = LogicalDevice.getQueue(PresentQueueFamilyIndex, PresentQueueIndex);
	}
	VT_CORE_TRACE("Vulkan graphics and present queues obtained ");

	/* ===============================================
	 *          Create Command Pool
	 * ===============================================
	 */
	{
		vk::Result Result;
		std::tie(Result, m_CmdPool) = LogicalDevice.createCommandPool(
			{
				.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
				.queueFamilyIndex = m_PhysicalDevice.GetGraphicsQueue().queueFamilyIndex
			});

		VK_CHECK(Result, vk::Result::eSuccess, "Failed to create command pool");
	}

	VT_CORE_TRACE("Vulkan Command pool created");


    /* ===============================================
     *          RenderPass
     * ===============================================
     */

	{
		m_RenderPass.Init(LogicalDevice);
        const auto [Result, DepthFormat] = m_PhysicalDevice.FindSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        VT_CORE_ASSERT(Result, "Failed to find depth format");

        // FIX : Renderpass should reference swapchain images
        std::vector<vk::AttachmentDescription> Attachment
        {
            { {
                  .format = m_Swapchain.GetInfo().imageFormat,
                  .samples = vk::SampleCountFlagBits::e1,
                  .loadOp = vk::AttachmentLoadOp::eClear,
                  .storeOp = vk::AttachmentStoreOp::eStore,
                  .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                  .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                  .initialLayout = vk::ImageLayout::eUndefined,
                  .finalLayout = vk::ImageLayout::ePresentSrcKHR
              },
              {
                  .format = DepthFormat,
                  .samples = vk::SampleCountFlagBits::e1,
                  .loadOp = vk::AttachmentLoadOp::eClear,
                  .storeOp = vk::AttachmentStoreOp::eDontCare,
                  .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                  .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                  .initialLayout = vk::ImageLayout::eUndefined,
                  .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal
              } }
        };

        std::vector<vk::AttachmentReference> ColorAttachmentReference
        {
            { { .attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal } }
        };

        std::vector<vk::AttachmentReference> DepthStencilAttachmentReference
        {
            { { .attachment = 1, .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal } } 
        };

        std::vector<vk::SubpassDescription> Subpass
        {
            { {
                .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
                .colorAttachmentCount = static_cast<uint32_t>(ColorAttachmentReference.size()),
                .pColorAttachments = ColorAttachmentReference.data(),
                .pDepthStencilAttachment = DepthStencilAttachmentReference.data()
            } }
        };

        std::vector<vk::SubpassDependency> SubpassDependency
        {
            { {
                .srcSubpass = vk::SubpassExternal,
                .dstSubpass = 0,
                .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                .srcAccessMask = vk::AccessFlagBits::eNone,
                .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead
            } }
        };


        m_RenderPass.Create(Attachment, Subpass, SubpassDependency);
    }

    VT_CORE_TRACE("Renderpass created");

    /* =====================================
     *        Image Resources
     * =====================================
     */
    CreateResources();

    /* =====================================
     *         Vertex && Index buffer
     * =====================================
     */

    BufferLayout VertexLayout {ShaderDataType::eFloat2, ShaderDataType::eFloat3};
    BufferLayout IndexLayout {ShaderDataType::eInt};
    {
        const auto PD_MemProperty = m_PhysicalDevice.Get().getMemoryProperties();

        std::vector<std::array<float, 5>> VertexData {
            {{0.0f, -0.5f, 1.0f, 0.0f, 0.0f},
             {0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
             {0.0f, 0.5f, 1.0f, 0.0f, 0.0f},
             {0.5f, -0.5f, 0.0f, 0.0f, 1.0f}}
        };

        std::vector IndexData {0, 1, 2, 0, 3, 1};

        vk::BufferCreateInfo VertexBufferInfo {
            .size  = VertexLayout.GetStride() * VertexData.size(),
            .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc |
                vk::BufferUsageFlagBits::eTransferDst,
            .sharingMode = vk::SharingMode::eExclusive};

        m_VertexBuffer.Create(VertexBufferInfo, LogicalDevice);
        m_VertexBuffer.BindMem(0, PD_MemProperty, vk::MemoryPropertyFlagBits::eDeviceLocal);

        vk::BufferCreateInfo IndexBufferInfo {
            .size  = IndexLayout.GetStride() * IndexData.size(),
            .usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferSrc |
                vk::BufferUsageFlagBits::eTransferDst,
            .sharingMode = vk::SharingMode::eExclusive};

        m_IndexBuffer.Create(IndexBufferInfo, LogicalDevice);
        m_IndexBuffer.BindMem(0, PD_MemProperty, vk::MemoryPropertyFlagBits::eDeviceLocal);

        VT_CORE_TRACE("Index and Vertex Buffer Created");

        UploadData(
            m_VertexBuffer.Get(),
            VertexData.data(),
            static_cast<uint32_t>(VertexData.size() * sizeof(VertexData[0])),
            0,
            m_GraphicQ,
            m_DrawFence.Get());
        UploadData(
            m_IndexBuffer.Get(),
            IndexData.data(),
            static_cast<uint32_t>(IndexData.size() * sizeof(IndexData[0])),
            0,
            m_GraphicQ,
            m_DrawFence.Get());
    }
    VT_CORE_TRACE("Index and Vertex Buffer data data uploaded");

    /* =====================================
     *         Shader modules
     * =====================================
     */

    {
        // Shader files
        std::array<HLSL::ShaderFileInfo, 2> ShaderFiles {
            {{{.FileDir = "Src/Shader", .FileName = "Vertex.hlsl"},
{L"-spirv", L"-E main", L"-T vs_6_3"},
vk::ShaderStageFlagBits::eVertex,
HLSL::DXC_FileEncodingACP},

             {{.FileDir = "Src/Shader", .FileName = "Fragment.hlsl"},
             {L"-spirv", L"-E main", L"-T ps_6_3"},
             vk::ShaderStageFlagBits::eFragment,
             HLSL::DXC_FileEncodingACP}}
        };

        BufferLayout CameraTransformLayout {ShaderDataType::eMat4, ShaderDataType::eMat4, ShaderDataType::eMat4};

        m_TriangleShader.Create(
            ShaderFiles,
            CameraTransformLayout,
            VertexLayout,
            m_RenderPass.Get(),
            m_PhysicalDevice.Get(),
            LogicalDevice);
    }

    VT_CORE_TRACE("Graphics Pipline Created");
}

Context::~Context()
{
	vk::Device LogicalDevice = m_LogicalDevice.Get();

	vk::Result Result = LogicalDevice.waitIdle();
	VK_CHECK(Result, vk::Result::eSuccess, "Failed to wait for device idle");

	LogicalDevice.freeCommandBuffers(m_CmdPool, m_DrawBuffer);
	LogicalDevice.destroyCommandPool(m_CmdPool);

	LogicalDevice.destroySemaphore(m_ImageAvailable);
	LogicalDevice.destroySemaphore(m_RenderFinished);
}

/* ==================================================================
 *                      Private
 * ==================================================================
 */

void Context::Resize(uint32_t Width, uint32_t Height)
{
    m_Swapchain.Resize(Width, Height);
    m_MaxFrameCount = m_Swapchain.GetMaxFrameCount();

    DestroyResources();
    CreateResources();
}


void Context::UploadData(vk::Buffer Dest, void* Data, uint32_t Size, uint32_t Offset, vk::Queue Queue, vk::Fence Fence)
    const
{
    vk::BufferCreateInfo StagingBufferInfo {
        .size        = Size,
        .usage       = vk::BufferUsageFlagBits::eTransferSrc,
        .sharingMode = vk::SharingMode::eExclusive};

    Buffer StagingBuffer;
    StagingBuffer.Create(StagingBufferInfo, m_LogicalDevice.Get());
    StagingBuffer.BindMem(
        0,
        m_PhysicalDevice.Get().getMemoryProperties(),
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    StagingBuffer.LoadData(Data, Size, {}, Offset);

    vk::BufferCopy Region {.srcOffset = 0, .dstOffset = Offset, .size = Size};

    StagingBuffer.CopyTo(Dest, Queue, m_CmdPool, Region);
}

void Context::CreateResources()
{
    /* ===============================================
     *          Create command buffer
     * ===============================================
     */
    {
        vk::CommandBufferAllocateInfo CommandBufferInfo{ .commandPool = m_CmdPool,
                                                         .level = vk::CommandBufferLevel::ePrimary,
                                                         .commandBufferCount = m_MaxFrameCount };

        vk::Result Result;
        std::tie(Result, m_DrawBuffer) = m_LogicalDevice.Get().allocateCommandBuffers(CommandBufferInfo);

        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create draw command buffer");
    }
    VT_CORE_TRACE("Vulkan draw buffer (cmd buffer) created");

    /* ===============================================
     *              FrameBuffer
     * ===============================================
     */
    {
        vk::FramebufferCreateInfo FrameBufferInfo{ .renderPass = m_RenderPass.Get(),
                                                   .width = m_Swapchain.GetInfo().imageExtent.width,
                                                   .height = m_Swapchain.GetInfo().imageExtent.height,
                                                   .layers = 1 };

        const auto ImageViews = m_Swapchain.GetImageView();
        m_FrameBuffer.resize(ImageViews.size());

        for (int i = 0; i < ImageViews.size(); ++i)
        {
            FrameBufferInfo.attachmentCount = static_cast<uint32_t>(ImageViews[i].size());
            FrameBufferInfo.pAttachments    = ImageViews[i].data();

            m_FrameBuffer[i].Create(m_LogicalDevice.Get(), FrameBufferInfo);
        }
    }
    VT_CORE_TRACE("Frame buffers created");
}

void Context::DestroyResources()
{
    m_LogicalDevice.Get().freeCommandBuffers(m_CmdPool, m_DrawBuffer);
    for (auto& FB : m_FrameBuffer) { FB.Destroy(); }

    VT_CORE_TRACE("Frame buffers destroyed");
}
} // namespace VT::Vulkan
