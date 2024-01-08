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

		DXC_ShaderFileInfo VertexShaderFile{};
		VertexShaderFile.FileLocation = "S:\Dev\Projects\VulkanTest\Src\Shader";
		VertexShaderFile.FileName = "Vertex.hlsl";
		VertexShaderFile.Stage = vk::ShaderStageFlagBits::eVertex;
		VertexShaderFile.CL_Args = L"-spirv";

		DXC_ShaderFileInfo FragmentShaderFile{};
		FragmentShaderFile.FileLocation = "S:\Dev\Projects\VulkanTest\Src\Shader";
		FragmentShaderFile.FileName = "Fragment.hlsl";
		FragmentShaderFile.Stage = vk::ShaderStageFlagBits::eFragment;
		FragmentShaderFile.CL_Args = VertexShaderFile.CL_Args;

		m_Renderer.createGraphicsPipeline("GraphicPipeline", { VertexShaderFile, FragmentShaderFile });
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
}
