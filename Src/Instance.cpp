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
	}

	void Instance::initDevice(GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions)
	{
		if (static_cast<vk::Result>(glfwCreateWindowSurface(m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface))) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to create window surface");
		}

		// find device
		if (!m_PhysicalDevice.findPhysicalDevice(m_VulkanInstance.enumeratePhysicalDevices(), RequiredExtensions)) { throw std::runtime_error("Cannot find compatible physical device"); }

		// add queue
		std::vector<std::tuple<vk::QueueFlagBits, float>> RequiredQueues = { {vk::QueueFlagBits::eGraphics, 0.f} };
		for (const auto& Q : RequiredQueues)
		{
			m_PhysicalDevice.addQueue(std::get<vk::QueueFlagBits>(Q), std::get<float>(Q));
		}

		m_PhysicalDevice.bindSurface(m_Surface);

		// find present queue
		if (!m_PhysicalDevice.findPresentQueue()) { throw std::runtime_error("Did not find present queue"); }

		// create device;
		m_LogicalDevice = m_PhysicalDevice.createLogicalDevice(RequiredExtensions);
	}

	PhysicalDevice Instance::getPhysicalDevice() const
	{
		return m_PhysicalDevice;
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
