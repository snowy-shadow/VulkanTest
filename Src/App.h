#pragma once

// enable aggregate initialization
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "Instance.h"
#include "Renderer.h"

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
	};
}