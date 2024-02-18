#include "Instance.h"

#ifndef NDEBUG
	#include "DebugMessenger.h"
#endif

#include <unordered_set>
#include <ranges>

namespace VT
{
	Instance::Instance(const vk::ApplicationInfo& ApplicationInfo) { initInstance(ApplicationInfo); }

	void Instance::initInstance(const vk::ApplicationInfo& ApplicationInfo)
	{
		// check that glfwinit() is called
		// assert();
		// Layers
		std::vector<const char*> Layers{};

		// Extentions
		uint32_t glfwExtensionCount{ 0 };
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };
		std::vector<const char*> Extentions{ glfwExtensions, glfwExtensions + glfwExtensionCount };

#ifndef NDEBUG
		Layers.emplace_back("VK_LAYER_KHRONOS_validation");
		Extentions.emplace_back("VK_EXT_debug_utils");
#endif

#ifdef __APPLE__
		Extentions.emplace_back("VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME");
#endif

		if (!isSupported(Extentions, Layers))
		{
			throw std::runtime_error("Instance does not support required extensions and layers");
		}

		// Instance info
		const vk::InstanceCreateInfo InstanceCreateInfo
		{
#ifdef __APPLE__
			.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
#endif
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

	void Instance::createDevice(std::string Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues)
	{
		if (static_cast<vk::Result>(glfwCreateWindowSurface(m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface))) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to create window surface");
		}

		// find device
		if (!m_PhysicalDevice.findPhysicalDevice(m_VulkanInstance.enumeratePhysicalDevices(), RequiredExtensions)) { throw std::runtime_error("Cannot find compatible physical device"); }

		// add queue
		for (const auto& Q : RequiredQueues)
		{
			m_PhysicalDevice.addQueue(std::get<vk::QueueFlagBits>(Q), std::get<float>(Q));
		}

		// find present queue
		if (!m_PhysicalDevice.findPresentQueue(m_Surface, 1.f, 1)) { throw std::runtime_error("Did not find present queue"); }

		// create device;
		const auto [_, Result] = m_LogicalDevices.try_emplace(Name, m_PhysicalDevice.createLogicalDevice(RequiredExtensions));

		if (!Result) { throw std::runtime_error("Logical Device name collision! Attempt to create logical device with name : " + std::move(Name)); }
	}

	std::tuple<std::unordered_map<std::string, vk::Device> const*, PhysicalDevice const*, vk::SurfaceKHR> Instance::getDeviceReferences()
	{
		return std::tuple{ &m_LogicalDevices, &m_PhysicalDevice, m_Surface };
	}

	Instance::~Instance()
	{
		for (auto& D : m_LogicalDevices | std::views::values) { D.destroy(); }

#ifndef NDEBUG
		m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLD_Instance);
#endif

		m_VulkanInstance.destroySurfaceKHR(m_Surface);
		m_VulkanInstance.destroy();
	}


	bool Instance::isSupported(std::vector<const char*> RequiredExtensions, std::vector<const char*> RequiredLayers) const
	{
		// extension
		auto InstanceExtensions{ vk::enumerateInstanceExtensionProperties()};
		std::unordered_set<std::string> ExtensionSupported{};
		for(const auto& Ext : InstanceExtensions)
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