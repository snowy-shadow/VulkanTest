#include "App.h"


namespace VT
{
	App::App()
	{
		
		m_VulkanInstance.initInstance({ .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) });

		if (m_VulkanInstance.CreateWindowSurface(m_Renderer.getWindow()) != vk::Result::eSuccess) { throw std::runtime_error("Failed to create windows surface"); }
	}

	App::~App()
	{
	}

	void VT::App::run()
	{
		m_Renderer.update();
	}




	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	
}
