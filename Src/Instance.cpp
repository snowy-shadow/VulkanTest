#include "Instance.h"


namespace VT
{
	void Instance::initInstance(const vk::ApplicationInfo ApplicationInfo)
	{
		// Layers
		std::vector<const char*> Layers{};

		// Extentions
		uint32_t glfwExtensionCount{ 0 };
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };
		std::vector<const char*> Extentions{glfwExtensions, glfwExtensions + glfwExtensionCount };

		#ifndef NDEBUG
		Layers.push_back("VK_LAYER_KHRONOS_validation");
		Extentions.push_back("VK_EXT_debug_utils");
		#endif

		if (!isSupported(Extentions, Layers)) throw std::runtime_error("Instance does not support required extensions and layers");

		// Instance info
		vk::InstanceCreateInfo InstanceCreateInfo
		{
			.pApplicationInfo = &ApplicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(Layers.size()),
			.ppEnabledLayerNames = Layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(Extentions.size()),
			.ppEnabledExtensionNames = Extentions.data()
		};

		m_VulkanInstance = vk::createInstance(InstanceCreateInfo);

#ifndef NDEBUG
		m_DLD_Instance = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);

		m_DebugMessenger = VT::createDebugMessenger(m_VulkanInstance, m_DLD_Instance,
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
#endif

		m_LogicalDevice = m_PhysicalDevice.createLogicalDevice(
			m_VulkanInstance.enumeratePhysicalDevices(),
			{ "VK_KHR_swapchain" }, 
			{ {vk::QueueFlagBits::eGraphics, 0.f} },
			m_Surface);
	}

	vk::Instance& Instance::getInstance()
	{
		return m_VulkanInstance;
	}

	vk::SurfaceKHR& Instance::getSurface()
	{
		return m_Surface;
	}

	vk::Result Instance::createWindowSurface(GLFWwindow* Window)
	{
		return static_cast<vk::Result>(glfwCreateWindowSurface(m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface)));
	}

	Instance::~Instance()
	{
		m_LogicalDevice.destroy();

#ifndef NDEBUG
		m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLD_Instance);
#endif
		
		m_VulkanInstance.destroySurfaceKHR(m_Surface);
		m_VulkanInstance.destroy();
	}


	bool Instance::isSupported(std::vector<const char*> RequiredExtensions, std::vector<const char*> RequiredLayers) const
	{
		// extention
		auto InstanceExtentions{ vk::enumerateInstanceExtensionProperties()};
		std::unordered_set<std::string> ExtensionSupported{};
		for(const auto& Ext : InstanceExtentions)
		{
			ExtensionSupported.insert(Ext.extensionName);
		}

		// layerD
		auto InstanceLayerProperties{ vk::enumerateInstanceLayerProperties() };
		std::unordered_set<std::string>  LayerSupported;
		for (const auto& Layer : InstanceLayerProperties)
		{
			LayerSupported.insert(Layer.layerName);
		}


		// if support extensions
		for (const auto& ReqExt : RequiredExtensions)
		{
			if(!ExtensionSupported.contains(ReqExt))
			{
				return false;
			}
		}

		// if support layers
		for (const auto& ReqLayer : RequiredLayers)
		{
			if(!LayerSupported.contains(ReqLayer))
			{
				return false;
			}
		}

		// if instance supports layers and extensions
		return true;
	}
}
