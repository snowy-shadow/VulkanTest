#include "App.h"
#include <vulkan/vulkan_structs.hpp>


namespace VT
{
	App::App()
	{
		uint32_t glfwExtensionCount;
		const char** GLFW_Extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> Required_GLFW_Extensions { GLFW_Extensions, GLFW_Extensions + glfwExtensionCount };
		m_VulkanInstance.initInstance( {.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) }, Required_GLFW_Extensions);
		m_VulkanInstance.createDevice({"Main Physical", "Main Logic"} ,m_Window.m_Window, { "VK_KHR_swapchain" }, { {vk::QueueFlagBits::eGraphics, 1.f} });

		auto Devices = m_VulkanInstance.getDeviceReferences({"Main Physical", "Main Logic"});
		std::tie(m_LogicDevice, m_PhysicalDevice, std::ignore) = Devices;
	
		m_Renderer.bindDevices(std::move(Devices));
		m_Renderer.bindWindow(m_Window);

		// createMainGraphicPipeline();
	}

	void VT::App::run()
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
	
		int Width, Height;
		glfwGetWindowSize(m_Window.m_Window, &Width, &Height);

	
		Swapchain MainSwapchain{};
		Swapchain::SurfaceCapabilities SC =
		{
			.minImageCount = 2,
			.imageExtent = {Width, Height},
			.arrayLayers = 0,
			.surfaceTransform = {vk::SurfaceTransformFlagBitsKHR::eIdentity},
			.compositeAlpha = { vk::CompositeAlphaFlagBitsKHR::eOpaque},
			.imageUsage = {vk::ImageUsageFlagBits::eColorAttachment}
		};

		SC = MainSwapchain.findSurfaceCapabilities(SC);
		auto SurfaceFormat = MainSwapchain.findSurfaceFormat({{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}});
	
		MainSwapchain.m_SwapchainInfo = vk::SwapchainCreateInfoKHR
		{
			.minImageCount = SC.minImageCount,
			.imageFormat = SurfaceFormat.format,
			.imageColorSpace = SurfaceFormat.colorSpace,
			.imageExtent = 	SC.imageExtent,
			.imageArrayLayers = SC.arrayLayers,
			.imageUsage = SC.imageUsage[0],
			.preTransform = SC.surfaceTransform[0],
			.compositeAlpha = SC.compositeAlpha[0],
			.presentMode = MainSwapchain.findPresentMode({vk::PresentModeKHR::eFifo}),
		};
	
		if(m_PhysicalDevice->graphicsQueueCanPresent())
		{
			MainSwapchain.m_SwapchainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			MainSwapchain.m_SwapchainInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_PhysicalDevice->getGraphicsPresentQueueIndices().size());
			MainSwapchain.m_SwapchainInfo.pQueueFamilyIndices = m_PhysicalDevice->getGraphicsPresentQueueIndices().data();
		}
	
		m_Renderer.createSwapChain("Main", MainSwapchain);
	}

	void App::createMainGraphicPipeline()
	{
		File::DXC_ShaderFileInfo VertexShaderFile{};
		VertexShaderFile.FileLocation = "/Users/kuang/Developer/Projects/VulkanTest/Src/Shader";
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

		int WindowWidth, WindowHeight;
		glfwGetFramebufferSize(m_Window.m_Window, &WindowWidth, &WindowHeight);
		std::vector<vk::Viewport> Viewports
		{
			{
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(WindowWidth),
				.height = static_cast<float>(WindowHeight),
				.minDepth = 0.f,
				.maxDepth = 1.f
			}
		};

		std::vector<vk::Rect2D> Scissors
		{
			{
				.offset =
				{
					.x = Viewports[0].x,
					.y = Viewports[0].y
				},

				.extent =
				{
					.width = static_cast<uint32_t>(Viewports[0].width),
					.height = static_cast<uint32_t>(Viewports[0].height)
				}
			}
		};

		vk::PipelineViewportStateCreateInfo ViewportStateInfo
		{
			.viewportCount = static_cast<uint32_t>(Viewports.size()),
			.pViewports = Viewports.data(),
			.scissorCount = static_cast<uint32_t>(Scissors.size()),
			.pScissors = Scissors.data()
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

		std::vector<vk::SubpassDescription> Subpasses
		{
			{
				
			},
		};

		std::vector<vk::SubpassDependency> SubpassDependencies
		{};
		
		std::vector<vk::AttachmentDescription> Attachements
		{
			
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

		vk::RenderPass RenderPass = m_LogicDevice.createRenderPass(RenderPassInfo);
		
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
			.layout = PipelineLayout,
			.renderPass = RenderPass
		};

		m_Renderer.createGraphicsPipeline("Main Pipeline", { VertexShaderFile }, MainGraphicPipelineInfo);
	}
}
