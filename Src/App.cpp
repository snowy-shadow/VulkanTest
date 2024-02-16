#include "App.h"


namespace VT
{
	App::App()
	{
		m_VulkanInstance.initInstance({ .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) });
		m_VulkanInstance.initDevice(m_Window.m_Window, { "VK_KHR_swapchain" }, { {vk::QueueFlagBits::eGraphics, 1.f} });

		m_Renderer.bindInstance(m_VulkanInstance);
		m_Renderer.bindWindow(m_Window);
		m_Renderer.createSwapChain({"Main"});

		createMainGraphicPipeline();
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

	void App::createMainGraphicPipeline()
	{
		File::DXC_ShaderFileInfo VertexShaderFile{};
		VertexShaderFile.FileLocation = "S:/Dev/Projects/VulkanTest/Src/Shader";
		VertexShaderFile.FileName = "Vertex.hlsl";
		VertexShaderFile.Stage = vk::ShaderStageFlagBits::eVertex;
		VertexShaderFile.CL_Args = { L"-spirv", L"-E main", L"-T vs_6_1" };

		/*File::DXC_ShaderFileInfo FragmentShaderFile{};
		FragmentShaderFile.FileLocation = "S:/Dev/Projects/VulkanTest/Src/Shader";
		FragmentShaderFile.FileName = "Fragment.hlsl";
		FragmentShaderFile.Stage = vk::ShaderStageFlagBits::eFragment;
		FragmentShaderFile.CL_Args = {L"-E main", L"-T ps_6_1", L"-spirv" };*/

		GraphicPipelineConfig MainGraphicPipelineInfo;

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

		MainGraphicPipelineInfo.m_VertexInputStateInfo =
		{
			.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexInputBindings.size()),
			.pVertexBindingDescriptions = VertexInputBindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributes.size()),
			.pVertexAttributeDescriptions = VertexAttributes.data()
		};

		MainGraphicPipelineInfo.m_InputAssemblyInfo =
		{
			.topology = vk::PrimitiveTopology::eTriangleList,
			.primitiveRestartEnable = vk::False,
		};

		int WindowWidth, WindowHeight;
		glfwGetFramebufferSize(m_Window.m_Window, &WindowWidth, &WindowHeight);
		MainGraphicPipelineInfo.m_Viewports =
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

		MainGraphicPipelineInfo.m_Scissors =
		{
			{
				.offset =
				{
					.x = 0,
					.y = 0
				},

				.extent =
				{
					.width = static_cast<uint32_t>(WindowWidth),
					.height = static_cast<uint32_t>(WindowHeight)
				}
			}
		};

		MainGraphicPipelineInfo.m_ViewportStateInfo =
		{
			.viewportCount = static_cast<uint32_t>(MainGraphicPipelineInfo.m_Viewports.size()),
			.pViewports = MainGraphicPipelineInfo.m_Viewports.data(),
			.scissorCount = static_cast<uint32_t>(MainGraphicPipelineInfo.m_Scissors.size()),
			.pScissors = MainGraphicPipelineInfo.m_Scissors.data()
		};

		MainGraphicPipelineInfo.m_RasterizationStateInfo =
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
		MainGraphicPipelineInfo.m_MultisampleStateInfo =
		{
			.rasterizationSamples = vk::SampleCountFlagBits::e1,
			.sampleShadingEnable = vk::False
		};

		MainGraphicPipelineInfo.m_ColorBlendAttachmentState =
		{
			{
				.blendEnable = vk::False,
			}
		};

		MainGraphicPipelineInfo.m_ColorBlendStateInfo = 
		{
			.logicOpEnable = vk::False,
			.attachmentCount = static_cast<uint32_t>(MainGraphicPipelineInfo.m_ColorBlendAttachmentState.size()),
			.pAttachments = MainGraphicPipelineInfo.m_ColorBlendAttachmentState.data()
		};

		MainGraphicPipelineInfo.m_DepthStencilStateInfo = 
		{
			.depthTestEnable = vk::True,
			.depthWriteEnable = vk::True,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		m_Renderer.createGraphicsPipeline("GraphicPipeline", { VertexShaderFile }, MainGraphicPipelineInfo);
	}
}
