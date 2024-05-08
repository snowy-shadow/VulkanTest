#include "Renderer.h"

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

		
	}

	void Renderer::bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices, Window* W)
	{
		std::tie(m_LogicalDevice, m_PhysicalDevice, m_Surface) = Devices;
		m_Window = W;
	}

	void Renderer::createMainGraphicsPipeline()
	{
		std::array<File::DXC_ShaderFileInfo, 1> ShaderFiles
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

			//{
			//	{
			//		.FileLocation = "Shader",
			//		.FileName = "Fragment.hlsl",
			//	},

			//	vk::ShaderStageFlagBits::eFragment,
			//	{L"-E main", L"-T ps_6_1", L"-spirv" }
			//}
		} };


		std::array<vk::VertexInputBindingDescription, 1> VertexInputBindings
		{ {
				{
				// binding 0
				.binding = 0,
				// pos, norm, tex coord
				.stride = sizeof(float) * (3 + 3 + 2),
				.inputRate = vk::VertexInputRate::eVertex
			}
	} };

		std::array<vk::VertexInputAttributeDescription, 3> VertexAttributes
		{ {
				// pos, 3 floats
				{
					.location = 0,
					.binding = VertexInputBindings[0].binding,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = 0
				},

			// norm, 3 floats
			{
				.location = 1,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = sizeof(float) * 3
			},
			// tex coord, 2 floats
			{
				.location = 2,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32Sfloat,
				.offset = sizeof(float) * (3 + 3)
			},
	} };


		const auto& SwapchainInfo{ m_Swapchain.getSwapchainCreateInfo().imageExtent };

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

		std::array<vk::Viewport, 1> Viewports
		{ {
			{
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(SwapchainInfo.width),
				.height = static_cast<float>(SwapchainInfo.height),
				.minDepth = 0.f,
				.maxDepth = 1.f
			}
		} };

		std::array<vk::Rect2D, 1> Scissors
		{ {

			{
				.offset =
				{
					.x = 0,
					.y = 0
				},
				.extent =
				{
					.width = SwapchainInfo.width,
					.height = SwapchainInfo.height,
				}
			}
		} };

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
			.cullMode = vk::CullModeFlagBits::eNone,
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
				.format = m_Swapchain.getSwapchainCreateInfo().imageFormat,
				.samples = vk::SampleCountFlagBits::e1,
				.loadOp = vk::AttachmentLoadOp::eDontCare,
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
			.minImageCount = 2,
			.imageExtent = {static_cast<unsigned int>(Width), static_cast<unsigned int>(Height)},
			.arrayLayers = 1,
			.surfaceFormat = {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}},
			.presentMode = {vk::PresentModeKHR::eFifo},
			.surfaceTransform = {vk::SurfaceTransformFlagBitsKHR::eIdentity},
			.compositeAlpha = { vk::CompositeAlphaFlagBitsKHR::eOpaque},
			.imageUsage = {vk::ImageUsageFlagBits::eColorAttachment}
		};

		createSwapChain("Main", true, {}, std::move(SwapchainQueries));
	}


	void Renderer::createSwapChain(std::string SwapchainName, bool GraphicsPresent, vk::SwapchainCreateInfoKHR SwapchainCreateInfo, Swapchain::Capabilities Queries)
	{
		const auto& CB = m_Swapchain.queryCapabilities(std::move(Queries), m_PhysicalDevice, m_Surface);

		SwapchainCreateInfo.minImageCount = CB.minImageCount;
		SwapchainCreateInfo.imageExtent = CB.imageExtent;
		SwapchainCreateInfo.imageFormat = CB.surfaceFormat[0].format;
		SwapchainCreateInfo.imageColorSpace = CB.surfaceFormat[0].colorSpace;
		SwapchainCreateInfo.imageUsage = CB.imageUsage[0];
		SwapchainCreateInfo.preTransform = CB.surfaceTransform[0];
		SwapchainCreateInfo.compositeAlpha = CB.compositeAlpha[0];
		SwapchainCreateInfo.presentMode = CB.presentMode[0];
		SwapchainCreateInfo.imageArrayLayers = CB.arrayLayers;
		SwapchainCreateInfo.surface = m_Surface;
		
		if(GraphicsPresent && !m_PhysicalDevice->graphicsQueueCanPresent())
		{
			SwapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_PhysicalDevice->getGraphicsPresentQueueIndices().size());
			SwapchainCreateInfo.pQueueFamilyIndices = m_PhysicalDevice->getGraphicsPresentQueueIndices().data();
		}

		m_Swapchain.createSwapchain(SwapchainCreateInfo, m_LogicalDevice);
	}

	bool Renderer::createPipelineLayout(std::string Name, const vk::PipelineLayoutCreateInfo& LayoutInfo)
	{
		return mDG_Pipeline.insert<vk::PipelineLayout>(m_LogicalDevice.createPipelineLayout(LayoutInfo),
			std::move(Name), [&](auto& Layout) {m_LogicalDevice.destroyPipelineLayout(Layout); }
		);
	}

	bool Renderer::createRenderPass(std::string Name, const vk::RenderPassCreateInfo& RenderPassInfo)
	{
		return mDG_Pipeline.insert<vk::RenderPass>(m_LogicalDevice.createRenderPass(RenderPassInfo),
			std::move(Name), [&](auto& Renderpass) {m_LogicalDevice.destroyRenderPass(Renderpass); }
		);
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
			|| mDG_Pipeline.has<vk::Pipeline>(GraphicsPipelineName)			// if name collision
			|| !mDG_Pipeline.has<vk::PipelineLayout>(PipelineLayoutName)	// dependent type not found
			|| !mDG_Pipeline.has<vk::RenderPass>(RenderPassName))			// dependent type not found
		{
			return false;
		}

		ShaderCompiler m_ShaderCompiler;
		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfos;
	
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

		PipelineInfo.renderPass = mDG_Pipeline.get<vk::RenderPass>(RenderPassName);
		PipelineInfo.layout = mDG_Pipeline.get<vk::PipelineLayout>(PipelineLayoutName);

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);

		// delete shader modules
		for(const auto& SM : ShaderStageInfos) { m_LogicalDevice.destroyShaderModule(SM.module); }
	
		// check Pipeline result
		if (Result != vk::Result::eSuccess) { return false; }

		return (mDG_Pipeline.insert<vk::Pipeline>(std::move(Pipeline), GraphicsPipelineName, [&](vk::Pipeline& p) { m_LogicalDevice.destroyPipeline(p); }) &&
			mDG_Pipeline.addDependency<vk::Pipeline, vk::PipelineLayout>(GraphicsPipelineName, PipelineLayoutName) &&
			mDG_Pipeline.addDependency<vk::Pipeline, vk::RenderPass>(GraphicsPipelineName, RenderPassName));
    }

	void Renderer::update()
	{

	}

	Renderer::~Renderer()
	{
		destroy();
	}


	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	void Renderer::destroy() noexcept
	{
		m_LogicalDevice.destroySwapchainKHR(m_Swapchain.getSwapchain());
	}
}

