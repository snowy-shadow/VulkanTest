#include "Instance.h"

#ifndef NDEBUG
	#include "DebugMessenger.h"
#endif

#include <unordered_set>

namespace VT
{
	void Instance::initInstance(const vk::ApplicationInfo& ApplicationInfo, std::vector<const char*> Extensions, std::vector<const char*> Layers)
	{
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
		m_ObjectConstructedMask = ObjectConstructed::eInstance;

#ifndef NDEBUG
		m_DLD_Instance = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);

		m_DebugMessenger = VT::createDebugMessenger(m_VulkanInstance, m_DLD_Instance,
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
		m_ObjectConstructedMask = ObjectConstructed::eDebugMessenger;
#endif
	}
	
	vk::Instance Instance::getInstance() const noexcept { return m_VulkanInstance; }
	
	Instance::~Instance()
	{
		
#ifndef NDEBUG
		if(m_ObjectConstructedMask & ObjectConstructed::eDebugMessenger) 
		{ m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLD_Instance); }
#endif
		
		if(m_ObjectConstructedMask & ObjectConstructed::eInstance) { m_VulkanInstance.destroy(); }
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
