#pragma once

#include "Instance.h"
#include "Renderer.h"
#include "Window.h"

namespace VT
{
	class App
	{
	public:
		App();
		~App();
		void run();

	private:
		Renderer m_Renderer;
		Instance m_VulkanInstance;

		Window m_Window{ {1280, 720}, "VulkanTest" };
	};
}