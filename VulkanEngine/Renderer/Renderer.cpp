#include "Renderer.h"

#include <iostream>

namespace VT
{
	Renderer::Renderer(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices, Window* W)
	{
		bindDevices(std::forward<std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR>>(Devices), W);
	}

	void Renderer::init()
	{
		createMainSwapChain();
		createMainGraphicsPipeline();
		ImageAvailable = m_LogicalDevice.createSemaphore({});
		RenderFinished = m_LogicalDevice.createSemaphore({});
		Fence = m_LogicalDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});

		createCommandBuffer();
		createImage();
		createVertexBuffer();

	}

	void Renderer::update()
	{
		/* =============================================================================================================
		 *									Setup
		 * =============================================================================================================
		 */
		vk::resultCheck(m_LogicalDevice.waitForFences(Fence, true, std::numeric_limits<uint64_t>::max()), "Fence wait timed out\n");
		Swapchain& SC = m_DependencyGraph.get<Swapchain>("SwapChain");

		vk::Result Result;
		uint32_t CurrentFrameImage;
		bool SwapChainRecreate{ false };
		{
			try
			{
				std::tie(Result, CurrentFrameImage) = m_LogicalDevice.acquireNextImageKHR(
					SC.getSwapchain(),
					std::numeric_limits<uint64_t>::max(),
					ImageAvailable,
					VK_NULL_HANDLE);
			}
			catch (const std::exception&)
			{
				recreateSwapchain("SwapChain");
				m_LogicalDevice.destroySemaphore(ImageAvailable);
				ImageAvailable = m_LogicalDevice.createSemaphore({});
				return;
			}

			switch (Result)
			{
			case vk::Result::eSuccess:
				break;
			case vk::Result::eSuboptimalKHR:
				recreateSwapchain("SwapChain");
				m_LogicalDevice.destroySemaphore(ImageAvailable);
				ImageAvailable = m_LogicalDevice.createSemaphore({});
				return;
				break;

			default:
				throw std::runtime_error("Acquire Next Image error");
			}
		}

		// reset
		auto& CommandBuffer = m_DependencyGraph.get<std::vector<vk::CommandBuffer>>("CB").at(m_CurrentFrame);
		CommandBuffer.reset();
		m_LogicalDevice.resetFences(Fence);

		/* =============================================================================================================
		 *									Render Prep
		 * =============================================================================================================
		 */
		// CmdBuffer setup
		CommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		recordRenderPass(CommandBuffer, CurrentFrameImage);
		CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_DependencyGraph.get<vk::Pipeline>("Main Pipeline"));

		// dynamic states
		vk::Viewport ViewPort
		{
			.x = 0.f,
			.y = 0.f,
			.width = static_cast<float>(SC.getSwapchainCreateInfo().imageExtent.width),
			.height = static_cast<float>(SC.getSwapchainCreateInfo().imageExtent.height),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};
		vk::Rect2D Scissor
		{
			.offset = { 0,0 },
			.extent = SC.getSwapchainCreateInfo().imageExtent
		};
		CommandBuffer.setViewport(0, ViewPort);
		CommandBuffer.setScissor(0, Scissor);
		// end dynamic states

		// bind Vertex Info
		std::array VertexBuffers{ m_DependencyGraph.get<vk::Buffer>("VertexBuffer") };
		vk::DeviceSize OffsetSize[]{ 0 };
		CommandBuffer.bindVertexBuffers(0, static_cast<uint32_t>(VertexBuffers.size()), VertexBuffers.data(), OffsetSize);
	
		CommandBuffer.draw(
			static_cast<uint32_t>(m_DependencyGraph.get<std::vector<std::array<float, 5>>>("Vertices").size()),
			1,
			0,
			0);

		CommandBuffer.endRenderPass();
		CommandBuffer.end();
		// End CB recording

		// Queue submit
		vk::Queue& Queue = m_DependencyGraph.get<vk::Queue>("Graphics+PresentQueue");
		vk::PipelineStageFlags RenderWaitMask[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::SubmitInfo RenderSubmit
		{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &ImageAvailable,
			.pWaitDstStageMask = &RenderWaitMask[0],
			.commandBufferCount = 1,
			.pCommandBuffers = &CommandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &RenderFinished
		};
		Queue.submit(RenderSubmit, Fence);
		// end Queue submit

		// Present Image
		vk::SwapchainKHR Swapchain = SC.getSwapchain();
		vk::PresentInfoKHR PresentInfo
		{
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &RenderFinished,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain,
			.pImageIndices = &CurrentFrameImage
		};

		try
		{
			Result = Queue.presentKHR(PresentInfo);
		}
		catch (const std::exception&) { recreateSwapchain("SwapChain"); return; }

		switch (Result)
		{
		case vk::Result::eSuccess:
			break;
		case vk::Result::eSuboptimalKHR:
			// change window size
			SwapChainRecreate = true;
			break;

		default:
			throw std::runtime_error("Present error");
			break;
		}
		
		// end present image
		m_CurrentFrame = ++m_CurrentFrame % m_MaxFrameCount;

		// Misc, things to do while waiting on gpu
		if(SwapChainRecreate)
		{
			recreateSwapchain("SwapChain");
			m_LogicalDevice.destroySemaphore(ImageAvailable);
			ImageAvailable = m_LogicalDevice.createSemaphore({});
		}
	}

	void Renderer::createMainGraphicsPipeline()
	{
		std::array<File::DXC_ShaderFileInfo, 2> ShaderFiles
		{ {
			{
				{
					.FileLocation = "Shader",
					.FileName = "Vertex.hlsl",
					.Encoding = File::eUTF8
				},
				vk::ShaderStageFlagBits::eVertex,
				{ L"-spirv", L"-E main", L"-T vs_6_3" }
			},

			{
				{
					.FileLocation = "Shader",
					.FileName = "Fragment.hlsl",
				},

				vk::ShaderStageFlagBits::eFragment,
				{L"-spirv", L"-E main", L"-T ps_6_3" }
			}
		} };


		std::array<vk::VertexInputBindingDescription, 1> VertexInputBindings
		{ {
				{
				// binding 0
				.binding = 0,
				// pos, norm, tex coord
				.stride = sizeof(float) * (2 + 3),
				.inputRate = vk::VertexInputRate::eVertex
			}
		} };

		std::array<vk::VertexInputAttributeDescription, 2> VertexAttributes
		{ {
				// pos, 2 floats
				{
					.location = 0,
					.binding = VertexInputBindings[0].binding,
					.format = vk::Format::eR32G32Sfloat,
					.offset = 0
				},

			// color, 3 floats
			{
				.location = 1,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = sizeof(float) * 2
			},
			//// tex coord, 2 floats
			//{
			//	.location = 2,
			//	.binding = VertexInputBindings[0].binding,
			//	.format = vk::Format::eR32G32Sfloat,
			//	.offset = sizeof(float) * (3 + 3)
			//},
		} };

		// const auto& SwapchainInfo{ m_DependencyGraph.get<Swapchain>("SwapChain").getSwapchainCreateInfo().imageExtent};

		vk::PipelineVertexInputStateCreateInfo VertexInputStateInfo
		{
			.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexInputBindings.size()),
			.pVertexBindingDescriptions = VertexInputBindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributes.size()),
			.pVertexAttributeDescriptions = VertexAttributes.data()
		};

		vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo
		{
			.topology = vk::PrimitiveTopology::eTriangleList,
			.primitiveRestartEnable = vk::False,
		};

		vk::PipelineTessellationStateCreateInfo TessellationStateInfo{};

		//std::array<vk::Viewport, 1> Viewports
		//{ {
		//	{
		//		.x = 0.f,
		//		.y = 0.f,
		//		.width = static_cast<float>(SwapchainInfo.width),
		//		.height = static_cast<float>(SwapchainInfo.height),
		//		.minDepth = 0.f,
		//		.maxDepth = 1.f
		//	}
		//} };

		//std::array<vk::Rect2D, 1> Scissors
		//{ {

		//	{
		//		.offset =
		//		{
		//			.x = 0,
		//			.y = 0
		//		},
		//		.extent =
		//		{
		//			.width = SwapchainInfo.width,
		//			.height = SwapchainInfo.height,
		//		}
		//	}
		//} };

		vk::PipelineViewportStateCreateInfo ViewportStateInfo
		{
			/*.viewportCount = static_cast<uint32_t>(Viewports.size()),
			.pViewports = Viewports.data(),
			.scissorCount = static_cast<uint32_t>(Scissors.size()),
			.pScissors = Scissors.data()*/

			// using dynamic states
			.viewportCount = 1,
			.scissorCount = 1,
		};

		std::array<vk::DynamicState, 2> DynamicStates
		{ {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		} };

		vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo
		{
			.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size()),
			.pDynamicStates = DynamicStates.data()
		};

		vk::PipelineRasterizationStateCreateInfo RasterizationStateInfo
		{
			.depthClampEnable = vk::False,
			.rasterizerDiscardEnable = vk::False,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eClockwise,
			.depthBiasEnable = vk::False,
			.lineWidth = 1.f
		};

		// Turned Off
		vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo
		{
			.rasterizationSamples = vk::SampleCountFlagBits::e1,
			.sampleShadingEnable = vk::False
		};

		std::array<vk::PipelineColorBlendAttachmentState, 1> ColorBlendAttachmentState
		{ {
			{
				.blendEnable = vk::False,
				.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
				.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
				.colorBlendOp = vk::BlendOp::eAdd,
				.srcAlphaBlendFactor = vk::BlendFactor::eOne,
				.dstAlphaBlendFactor = vk::BlendFactor::eZero,
				.alphaBlendOp = vk::BlendOp::eAdd,
				.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			}
		} };

		vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo
		{
			.logicOpEnable = vk::False,
			.attachmentCount = static_cast<uint32_t>(ColorBlendAttachmentState.size()),
			.pAttachments = ColorBlendAttachmentState.data()
		};

		vk::PipelineDepthStencilStateCreateInfo DepthStencilStateInfo
		{
			.depthTestEnable = vk::True,
			.depthWriteEnable = vk::True,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		// vk::PipelineLayout PipelineLayout{};

		std::array<vk::AttachmentDescription, 1> Attachements
		{ {
			{
				.format = m_DependencyGraph.get<Swapchain>("SwapChain").getSwapchainCreateInfo().imageFormat,
				.samples = vk::SampleCountFlagBits::e1,
				.loadOp = vk::AttachmentLoadOp::eClear,
				.storeOp = vk::AttachmentStoreOp::eStore,
				.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
				.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
				.initialLayout = vk::ImageLayout::eUndefined,
				.finalLayout = vk::ImageLayout::ePresentSrcKHR
			}

		} };

		std::array<vk::AttachmentReference, 1> ColorAttachmentReferences
		{ {
			{
				.attachment = 0,
				.layout = vk::ImageLayout::eColorAttachmentOptimal
			}
		} };
		std::array<vk::SubpassDescription, 1> Subpasses
		{ {
			{
				.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
				.colorAttachmentCount = static_cast<uint32_t>(ColorAttachmentReferences.size()),
				.pColorAttachments = ColorAttachmentReferences.data(),
			},
		} };

		std::array<vk::SubpassDependency, 1> SubpassDependencies
		{ {
			{
				.srcSubpass = vk::SubpassExternal,
				.dstSubpass = 0,
				.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.srcAccessMask = vk::AccessFlagBits::eNone,
				.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
			},
		} };

		vk::RenderPassCreateInfo RenderPassInfo
		{
			.attachmentCount = static_cast<uint32_t>(Attachements.size()),
			.pAttachments = Attachements.data(),
			.subpassCount = static_cast<uint32_t>(Subpasses.size()),
			.pSubpasses = Subpasses.data(),
			.dependencyCount = static_cast<uint32_t>(SubpassDependencies.size()),
			.pDependencies = SubpassDependencies.data()
		};

		assert(createRenderPass("Main RenderPass", RenderPassInfo));
		assert(createPipelineLayout("Main PipelineLayout", {}));

		vk::GraphicsPipelineCreateInfo MainGraphicPipelineInfo
		{
			.pVertexInputState = &VertexInputStateInfo,
			.pInputAssemblyState = &InputAssemblyInfo,
			.pTessellationState = &TessellationStateInfo,
			.pViewportState = &ViewportStateInfo,
			.pRasterizationState = &RasterizationStateInfo,
			.pMultisampleState = &MultisampleStateInfo,
			.pDepthStencilState = &DepthStencilStateInfo,
			.pColorBlendState = &ColorBlendStateInfo,
			.pDynamicState = &DynamicStateCreateInfo,
			.subpass = 0
		};

		if (!createGraphicsPipeline(ShaderFiles, "Main Pipeline", MainGraphicPipelineInfo, "Main PipelineLayout", "Main RenderPass"))
		{
			throw std::runtime_error("Graphics pipeline creation failed");
		}

	}

	void Renderer::createMainSwapChain()
	{
		// get current window dimensions
		int Width, Height;
		glfwGetWindowSize(m_Window->m_Window, &Width, &Height);

		Swapchain::Capabilities SwapchainQueries
		{
			.minImageCount = m_MaxFrameCount,
			.imageExtent = {static_cast<unsigned int>(Width), static_cast<unsigned int>(Height)},
			.arrayLayers = 1,
			.surfaceFormat = {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}},
			.presentMode = {vk::PresentModeKHR::eFifo},
			.surfaceTransform = {vk::SurfaceTransformFlagBitsKHR::eIdentity},
			.compositeAlpha = { vk::CompositeAlphaFlagBitsKHR::eOpaque},
		};

		createSwapChain(true, { .imageUsage = vk::ImageUsageFlagBits::eColorAttachment }, std::move(SwapchainQueries));

		auto [QueueFamilyIndex, QueueIndex] = m_PhysicalDevice->getGraphicsPresentQueueIndices();

		m_DependencyGraph.insert<vk::Queue>(m_LogicalDevice.getQueue(QueueFamilyIndex, QueueIndex), "Graphics+PresentQueue");
	}

	void Renderer::recreateSwapchain(std::string Name)
	{
		Swapchain& SC = m_DependencyGraph.get<Swapchain>(Name);

		m_LogicalDevice.waitIdle();

		m_DependencyGraph.remove<VT::Swapchain>(Name, true);

		// recreate swapchain
		int Width = 0, Height = 0;
		glfwGetFramebufferSize(m_Window->m_Window, &Width, &Height);

		auto Info = SC.getSwapchainCreateInfo();
		Info.setImageExtent({ static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) });

		// update surface capabilities, else it will complain
		// cast to void, ignore output - warn unused variable
		(void) m_PhysicalDevice->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface);

		SC.createSwapchain(Info, m_LogicalDevice);
		// recreate
		createImage();
	}

	std::vector<vk::CommandBuffer>& Renderer::createCommandBuffer()
	{
		const auto& CP = m_DependencyGraph.insert<vk::CommandPool>
		(
			m_LogicalDevice.createCommandPool(
			{
				.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
				.queueFamilyIndex = m_PhysicalDevice->getGraphicsPresentQueueIndices().first
			}),
			"CommandPool",
			[&](vk::CommandPool& Pool) { m_LogicalDevice.destroyCommandPool(Pool); }
		);

		if (!CP.second) { throw std::runtime_error("Command Pool creation failed\n"); }

		vk::CommandBufferAllocateInfo CommandBufferInfo
		{
			.commandPool = CP.first,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 2
		};

		auto& Buffers = m_DependencyGraph.insert<std::vector<vk::CommandBuffer>>
		(m_LogicalDevice.allocateCommandBuffers(CommandBufferInfo),
			"CB",
			[&, &CP = CP.first](std::vector<vk::CommandBuffer>& CB)
			{
				m_LogicalDevice.freeCommandBuffers(CP, CB);
			}
		).first;

		if(!m_DependencyGraph.addDependency<std::vector<vk::CommandBuffer>, vk::CommandPool>("CB", "CommandPool"))
		{
			throw std::runtime_error("Failed to add command pool dependency");
		}

		return Buffers;
	}

	void Renderer::createImage()
	{
		const auto& SC = m_DependencyGraph.get<Swapchain>("SwapChain");
		const auto& SC_Info = SC.getSwapchainCreateInfo();

		// Image group setup
		auto SC_Images = m_LogicalDevice.getSwapchainImagesKHR(SC.getSwapchain());

		vk::ImageViewCreateInfo ImageViewInfo
		{
			.viewType = vk::ImageViewType::e2D,
			.format = SC_Info.imageFormat,
			.components = {},
			.subresourceRange =
			{
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		// Framebuffer info
		vk::FramebufferCreateInfo FB_Info
		{
			.renderPass = m_DependencyGraph.get<vk::RenderPass>("Main RenderPass"),
			.width = SC_Info.imageExtent.width,
			.height = SC_Info.imageExtent.height,
			.layers = 1
		};

		for(size_t i = 0; i < SC_Images.size(); i++)
		{
			ImageGroup& Img = m_Images.emplace_back();
			Img.bindDevice(m_LogicalDevice);

			// insert image
			Img.Images.emplace_back(SC_Images[i]);

			// create image view
			std::array ViewInfo = { ImageViewInfo };
			ViewInfo[0].image = Img.Images[0];
			Img.createViews(ViewInfo);

			// fill in framebuffer info
			FB_Info.attachmentCount = static_cast<uint32_t>(Img.ImageViews.size());
			FB_Info.pAttachments = Img.ImageViews.data();

			// insert into graph
			if(!m_DependencyGraph.insert<vk::Framebuffer>
			(
				m_LogicalDevice.createFramebuffer(FB_Info),
				"SwapChain FrameBuffer" + std::to_string(i),
				[&](vk::Framebuffer& FB) { m_LogicalDevice.destroyFramebuffer(FB); }
			).second)
			{
				throw std::runtime_error("Failed insert framebuffer");
			}

			if(!m_DependencyGraph.addDependency<vk::Framebuffer, Swapchain>("SwapChain FrameBuffer" + std::to_string(i), "SwapChain"))
			{
				throw std::runtime_error("Failed to add Swapchain dependency to framebuffer");
			}
		}

		
	}

	void Renderer::createVertexBuffer()
	{
		auto& Vertices = m_DependencyGraph.insert<std::vector<std::array<float, 5>>>(
			{
				{0.0f, -0.5f, 1.0f, 0.0f, 0.0f},
				{0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
				{-0.5f, 0.5f, 0.0f, 0.0f, 1.0f}
			}, "Vertices").first;


		// create buffer
		vk::BufferCreateInfo VertexBufferInfo
		{
			.size = Vertices.size() * Vertices[0].size() * sizeof(float),
			.usage = vk::BufferUsageFlagBits::eVertexBuffer,
			.sharingMode = vk::SharingMode::eExclusive
		};

		vk::Buffer& VertexBuffer = m_DependencyGraph.insert<vk::Buffer>(m_LogicalDevice.createBuffer(VertexBufferInfo),
			"VertexBuffer", 
			[&](vk::Buffer B) {m_LogicalDevice.destroyBuffer(B); }
		).first;

		auto MemReq = m_LogicalDevice.getBufferMemoryRequirements(VertexBuffer);

		vk::MemoryAllocateInfo MemAllocInfo
		{
			.allocationSize = MemReq.size,
			.memoryTypeIndex = findMemoryTypeIndex(MemReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
		};

		vk::DeviceMemory& VertexMem = m_DependencyGraph.insert<vk::DeviceMemory>(
			m_LogicalDevice.allocateMemory(MemAllocInfo),
			"VertexBuffer",
			[&](vk::DeviceMemory& Mem) {m_LogicalDevice.freeMemory(Mem); }
		).first;

		m_DependencyGraph.addDependency<vk::DeviceMemory, vk::Buffer>("VertexBuffer", "VertexBuffer");
		
		m_LogicalDevice.bindBufferMemory(VertexBuffer, VertexMem, 0);

		// Fill Buffer
		auto pVertexMem = m_LogicalDevice.mapMemory(VertexMem, 0, VertexBufferInfo.size);
		std::memcpy(pVertexMem, Vertices.data(), VertexBufferInfo.size);
		m_LogicalDevice.unmapMemory(VertexMem);

	}

	void Renderer::recordRenderPass(const vk::CommandBuffer& CB, uint32_t Frame)
	{
		std::array<vk::ClearValue, 1> ClearColor
		{ {
				{
					.color =
				{
						.float32 = {{0.f, 0.f, 0.f, 1.f} }
					}
				}
		} };

		vk::RenderPassBeginInfo RenderPassBeginInfo
		{
			.renderPass = m_DependencyGraph.get<vk::RenderPass>("Main RenderPass"),
			.framebuffer = m_DependencyGraph.get<vk::Framebuffer>("SwapChain FrameBuffer" + std::to_string(Frame)),
			.renderArea =
			{
				.offset = {0, 0},
				.extent = m_DependencyGraph.get<Swapchain>("SwapChain").getSwapchainCreateInfo().imageExtent
			},

			.clearValueCount = static_cast<uint32_t>(ClearColor.size()),
			.pClearValues = ClearColor.data()
		};
		CB.beginRenderPass(RenderPassBeginInfo, vk::SubpassContents::eInline);
	}

	void Renderer::createSwapChain(bool GraphicsPresent, vk::SwapchainCreateInfoKHR SwapchainCreateInfo, Swapchain::Capabilities Queries)
	{
		auto& SwapChain = m_DependencyGraph.insert<Swapchain>({}, "SwapChain").first;

		SwapChain.queryCapabilities(SwapchainCreateInfo, std::move(Queries), m_PhysicalDevice->getPhysicalDevice(), m_Surface);

		uint32_t QueueFamilyIndices[]{ m_PhysicalDevice->getGraphicsPresentQueueIndices().first };
		if(GraphicsPresent && !m_PhysicalDevice->graphicsQueueCanPresent())
		{

			SwapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			SwapchainCreateInfo.queueFamilyIndexCount = 1;
			SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
		}
		SwapChain.createSwapchain(SwapchainCreateInfo, m_LogicalDevice);
		m_MaxFrameCount = SwapchainCreateInfo.minImageCount;
	}

	bool Renderer::createPipelineLayout(std::string Name, const vk::PipelineLayoutCreateInfo& LayoutInfo)
	{
		return m_DependencyGraph.insert<vk::PipelineLayout>(m_LogicalDevice.createPipelineLayout(LayoutInfo),
			std::move(Name), [&](auto& Layout) { m_LogicalDevice.destroyPipelineLayout(Layout); }
		).second;
	}

	bool Renderer::createRenderPass(std::string Name, const vk::RenderPassCreateInfo& RenderPassInfo)
	{
		return m_DependencyGraph.insert<vk::RenderPass>(m_LogicalDevice.createRenderPass(RenderPassInfo),
			std::move(Name), [&](auto& Renderpass) { m_LogicalDevice.destroyRenderPass(Renderpass); }
		).second;
	}

	uint32_t Renderer::findMemoryTypeIndex(uint32_t TypeFilter, vk::MemoryPropertyFlags Usage) const
	{
		auto MemProperties = m_PhysicalDevice->getPhysicalDevice().getMemoryProperties();

		for(uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
		{
			if((TypeFilter & 1 << i) && (Usage & MemProperties.memoryTypes[i].propertyFlags) == Usage)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memeory type index\n");
	}

	bool Renderer::createGraphicsPipeline(
		std::span<const File::DXC_ShaderFileInfo> ShaderFiles,
		const std::string& GraphicsPipelineName,
		vk::GraphicsPipelineCreateInfo PipelineInfo,
		const std::string& PipelineLayoutName,
		const std::string& RenderPassName)
    {
		// cannot have name collision
		if(!m_LogicalDevice
			|| m_DependencyGraph.has<vk::Pipeline>(GraphicsPipelineName)			// if name collision
			|| !m_DependencyGraph.has<vk::PipelineLayout>(PipelineLayoutName)	// dependent type not found
			|| !m_DependencyGraph.has<vk::RenderPass>(RenderPassName))			// dependent type not found
		{
			return false;
		}

		ShaderCompiler m_ShaderCompiler;
		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo>& ShaderStageInfos = 
			m_DependencyGraph.insert<std::vector<vk::PipelineShaderStageCreateInfo>>(
				{},
				"ShaderModules",
				[&](auto& SSI) {for (const auto& SM : SSI) { m_LogicalDevice.destroyShaderModule(SM.module); }}
		).first;


		// load all shader spv
		for(std::size_t i = 0; i < ShaderSpvs.size(); i++)
		{
			ShaderStageInfos.push_back
			({
				.stage = ShaderFiles[i].Stage,
				.module = m_LogicalDevice.createShaderModule
				(
					{
						.codeSize = ShaderSpvs[i].size(),
						.pCode = reinterpret_cast<uint32_t*>(ShaderSpvs[i].data())
					}
				),

				.pName = "main"
			});
		}

		PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStageInfos.size());
		PipelineInfo.pStages = ShaderStageInfos.data();

		PipelineInfo.renderPass = m_DependencyGraph.get<vk::RenderPass>(RenderPassName);
		PipelineInfo.layout = m_DependencyGraph.get<vk::PipelineLayout>(PipelineLayoutName);

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);
	
		// check Pipeline result
		if (Result != vk::Result::eSuccess) { return false; }

		return
		(m_DependencyGraph.insert<vk::Pipeline>(std::move(Pipeline), GraphicsPipelineName, [&](const vk::Pipeline& p) { m_LogicalDevice.destroyPipeline(p); }).second &&
			m_DependencyGraph.addDependency<vk::Pipeline, vk::PipelineLayout>(GraphicsPipelineName, PipelineLayoutName) &&
			m_DependencyGraph.addDependency<vk::Pipeline, vk::RenderPass>(GraphicsPipelineName, RenderPassName));
    }

	void Renderer::bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices, Window* W)
	{
		std::tie(m_LogicalDevice, m_PhysicalDevice, m_Surface) = Devices;
		m_Window = W;
	}

	Renderer::~Renderer()
	{
		m_LogicalDevice.waitIdle();
		m_LogicalDevice.destroySemaphore(ImageAvailable);
		m_LogicalDevice.destroySemaphore(RenderFinished);
		m_LogicalDevice.destroyFence(Fence);
	}


	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	void Renderer::destroy() noexcept
	{
	}
}

