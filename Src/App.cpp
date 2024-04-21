#include "App.h"

// enable aggregate initialization
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_structs.hpp>


#include <iostream>
namespace VT
{
	App::App()
	{
		uint32_t glfwExtensionCount;
		const char** GLFW_Extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		m_VulkanInstance.initInstance( {.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) }, { GLFW_Extensions, GLFW_Extensions + glfwExtensionCount }, {});

		m_VulkanDevice.bindInstance(m_VulkanInstance.getInstance());
		m_VulkanDevice.createDevice("Main", m_Window.m_Window, { "VK_KHR_swapchain" }, { {vk::QueueFlagBits::eGraphics, 1.f} });

		m_Renderer.bindDevices(m_VulkanDevice.getDeviceReferences("Main"));
		m_Renderer.bindWindow(m_Window);

		createMainSwapchain();
		createMainGraphicPipeline();
	}

	void App::run()
	{
		while (!glfwWindowShouldClose(m_Window.m_Window))
		{
			glfwPollEvents();
		}
	}

	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	void App::createMainSwapchain()
	{
		// get current window dimensions
		int Width, Height;
		glfwGetWindowSize(m_Window.m_Window, &Width, &Height);

		const Swapchain::Capabilities SwapchainQueries
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

		m_Renderer.createSwapChain("Main", {}, std::move(SwapchainQueries));
	}

	void App::createMainGraphicPipeline()
	{
		File::DXC_ShaderFileInfo VertexShaderFile{};
		VertexShaderFile.FileLocation = "Shader";
		VertexShaderFile.FileName = "Vertex.hlsl";
		VertexShaderFile.Stage = vk::ShaderStageFlagBits::eVertex;
		VertexShaderFile.CL_Args = { L"-spirv", L"-E main", L"-T vs_6_3" };

		/*File::DXC_ShaderFileInfo FragmentShaderFile{};
		FragmentShaderFile.FileLocation = "S:/Dev/Projects/VulkanTest/Src/Shader";
		FragmentShaderFile.FileName = "Fragment.hlsl";
		FragmentShaderFile.Stage = vk::ShaderStageFlagBits::eFragment;
		FragmentShaderFile.CL_Args = {L"-E main", L"-T ps_6_1", L"-spirv" };*/

		std::vector<vk::VertexInputBindingDescription> VertexInputBindings
		{
			{
				.binding = 0,
				// pos, norm, tex coord
				.stride = sizeof(float) * (3 + 3 + 2),
				.inputRate = vk::VertexInputRate::eVertex
			}
		};

		std::vector<vk::VertexInputAttributeDescription> VertexAttributes
		{
			// pos
			{
				.location = 0,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = 0
			},

			// norm
			{
				.location = 1,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = sizeof(float) * 3
			},

			// tex coord
			{
				.location = 2,
				.binding = VertexInputBindings[0].binding,
				.format = vk::Format::eR32G32Sfloat,
				.offset = sizeof(float) * (3 + 3)
			}
		};


		const auto& SwapchainInfo{ m_Renderer.getSwapchainInfo("Main").imageExtent };

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

		std::vector<vk::Viewport> Viewports
		{
			{
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(SwapchainInfo.width),
				.height = static_cast<float>(SwapchainInfo.height),
				.minDepth = 0.f,
				.maxDepth = 1.f
			}
		};

		std::vector<vk::Rect2D> Scissors
		{
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
		};

		vk::PipelineViewportStateCreateInfo ViewportStateInfo
		{
			// using dynamic states
			.viewportCount = static_cast<uint32_t>(Viewports.size()),
			// .pViewports = Viewports.data(),
			.scissorCount = static_cast<uint32_t>(Scissors.size()),
			// .pScissors = Scissors.data()
		};

		std::vector<vk::DynamicState> DynamicStates
		{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

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

		std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachmentState
		{
			{
				.blendEnable = vk::False,
			}
		};

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

		vk::PipelineLayout PipelineLayout{};

		
		std::vector<vk::AttachmentDescription> Attachements
		{
			{
				.format = m_Renderer.getSwapchainInfo("Main").imageFormat,
				.samples = vk::SampleCountFlagBits::e1,
				.loadOp = vk::AttachmentLoadOp::eDontCare,
				.storeOp = vk::AttachmentStoreOp::eStore,
				.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
				.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
				.initialLayout = vk::ImageLayout::eUndefined,
				.finalLayout = vk::ImageLayout::ePresentSrcKHR
			}
			
		};
	
		std::vector<vk::AttachmentReference> ColorAttachmentReferences
		{
			{
				.attachment = 0,
				.layout = vk::ImageLayout::eColorAttachmentOptimal
			}
		};
		std::vector<vk::SubpassDescription> Subpasses
		{
			vk::SubpassDescription
			{
				.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
				.colorAttachmentCount = static_cast<uint32_t>(ColorAttachmentReferences.size()),
				.pColorAttachments = ColorAttachmentReferences.data(),
			},
		};

		std::vector<vk::SubpassDependency> SubpassDependencies
		{
			{
				.srcSubpass = vk::SubpassExternal,
				.dstSubpass = 0,
				.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.srcAccessMask = vk::AccessFlagBits::eNone,
				.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
			},
		};
	
		vk::RenderPassCreateInfo RenderPassInfo
		{
			.attachmentCount = static_cast<uint32_t>(Attachements.size()),
			.pAttachments = Attachements.data(),
			.subpassCount = static_cast<uint32_t>(Subpasses.size()),
			.pSubpasses = Subpasses.data(),
			.dependencyCount = static_cast<uint32_t>(SubpassDependencies.size()),
			.pDependencies = SubpassDependencies.data()
		};

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
			.subpass = 0
		};

		m_Renderer.createGraphicsPipeline("Main Pipeline", { VertexShaderFile }, {}, RenderPassInfo, MainGraphicPipelineInfo);
	}
}
