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

	App::~App()
	{
		m_Renderer.destroy();
		m_VulkanInstance.destroy();
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
		VertexShaderFile.CL_Args = L"-spirv";

		File::DXC_ShaderFileInfo FragmentShaderFile{};
		FragmentShaderFile.FileLocation = "S:/Dev/Projects/VulkanTest/Src/Shader";
		FragmentShaderFile.FileName = "Fragment.hlsl";
		FragmentShaderFile.Stage = vk::ShaderStageFlagBits::eFragment;
		FragmentShaderFile.CL_Args = VertexShaderFile.CL_Args;

		GraphicPipeline MainGraphicPipelineInfo;

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

		};

		m_Renderer.createGraphicsPipeline("GraphicPipeline", { VertexShaderFile, FragmentShaderFile }, MainGraphicPipelineInfo);
	}
}
