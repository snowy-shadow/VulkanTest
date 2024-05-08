#pragma once

#include "Instance.h"
#include "Device.h"
#include "Renderer.h"
#include "Window.h"

namespace VT
{
	class App
	{
	public:
		App();
		void run();

	private:

		Instance m_VulkanInstance;
		Device m_VulkanDevice;
		Renderer m_Renderer;
	
		Window m_Window{ {1280, 720}, "VulkanTest" };
	};
}
