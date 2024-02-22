#include "Instance.h"

#ifndef NDEBUG
	#include "DebugMessenger.h"
#endif

#include <ranges>
#include <unordered_set>

namespace VT
{
	void Instance::initInstance(const vk::ApplicationInfo& ApplicationInfo, std::vector<const char*> Extensions)
	{
		// Layers
		std::vector<const char*> Layers{};

#ifndef NDEBUG
		Layers.emplace_back("VK_LAYER_KHRONOS_validation");
		Extensions.emplace_back("VK_EXT_debug_utils");
#endif

#ifdef __APPLE__
		Extensions.emplace_back("VK_KHR_portability_enumeration");
#endif

		if (!isSupported(Extensions, Layers))
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
			.enabledExtensionCount = static_cast<uint32_t>(Extensions.size()),
			.ppEnabledExtensionNames = Extensions.data()
		};

		m_VulkanInstance = vk::createInstance(InstanceCreateInfo);
		m_ObjectConstructed = 0b1;

#ifndef NDEBUG
		m_DLD_Instance = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);

		m_DebugMessenger = VT::createDebugMessenger(m_VulkanInstance, m_DLD_Instance,
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
		m_ObjectConstructed |= 0b10;
#endif
	}

	void Instance::createDevice(std::array<std::string, 2> Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues)
	{
		// m_VulkanInstance created
		assert(m_ObjectConstructed & 0b1);
		
		auto [PhysicalDeviceIter, Result] = m_PhysicalDevices.try_emplace(Name[0]);
		if(!Result) { throw std::runtime_error("Physical device name collision : " + std::move(Name[0])); }
		auto& PhysicalDevice = PhysicalDeviceIter->second;
		
		if (static_cast<vk::Result>(glfwCreateWindowSurface(m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface))) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to create window surface");
		}

		m_ObjectConstructed |= 0b100;
		// find device
		if (!PhysicalDevice.findPhysicalDevice(m_VulkanInstance.enumeratePhysicalDevices(), RequiredExtensions)) { throw std::runtime_error("Cannot find compatible physical device"); }

		// add queue
		for (const auto& Q : RequiredQueues)
		{
			PhysicalDevice.addQueue(std::get<vk::QueueFlagBits>(Q), std::get<float>(Q));
		}

		// find present queue
		if (!PhysicalDevice.findPresentQueue(m_Surface, 1.f, 1)) { throw std::runtime_error("Did not find present queue"); }

        auto Extensions = RequiredExtensions;
        if(PhysicalDevice.supportsPortabilitySubset()) { Extensions.emplace_back("VK_KHR_portability_subset"); }
		// create device;
		std::tie(std::ignore, Result) = m_LogicalDevices.try_emplace(Name[1], PhysicalDevice.createLogicalDevice(Extensions));

		if (!Result) { throw std::runtime_error("Logical Device name collision! Attempt to create logical device with name : " + std::move(Name[1])); }
	}

	std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Instance::getDeviceReferences(std::array<std::string, 2> Name)
	{
		auto PD_Iter = m_PhysicalDevices.find(Name[0]);
		if(PD_Iter == m_PhysicalDevices.cend()) { throw std::runtime_error("No logical device with name : " + std::move(Name[0])); }
		
		auto LD_Iter = m_LogicalDevices.find(Name[1]);
		if(LD_Iter == m_LogicalDevices.cend()) { throw std::runtime_error("No logical device with name : " + std::move(Name[1])); }
		
		return std::tuple{ LD_Iter->second, &PD_Iter->second, m_Surface };
	}
	
	Instance::~Instance()
	{
		for(auto& D : m_LogicalDevices | std::views::values) { D.destroy(); }

		if(m_ObjectConstructed & 0b100)  { m_VulkanInstance.destroySurfaceKHR(m_Surface); }
		
#ifndef NDEBUG
		if(m_ObjectConstructed & 0b010) 
		{ m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLD_Instance); }
#endif
		
		if(m_ObjectConstructed & 0b001) { m_VulkanInstance.destroy(); }
	}


	/* ==========================================
	 *				  Private
	 * ==========================================
	*/
	bool Instance::isSupported(std::vector<const char*> RequiredExtensions, std::vector<const char*> RequiredLayers) const
	{
		// extension
		auto InstanceExtensions{ vk::enumerateInstanceExtensionProperties()};
		std::unordered_set<std::string> ExtensionSupported{};
		for(const auto& Ext : InstanceExtensions)
		{
			ExtensionSupported.insert(Ext.extensionName);
		}

		// layer
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
