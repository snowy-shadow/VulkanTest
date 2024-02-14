#pragma once

// enable aggregate initialization
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <iosfwd>


namespace VT
{
	VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT, // MessageSeverity
		VkDebugUtilsMessageTypeFlagsEXT, // MessageType
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* // pUserData
	);

	vk::DebugUtilsMessengerEXT createDebugMessenger(
		const vk::Instance& Instance, 
		const vk::DispatchLoaderDynamic& DLD,
		vk::DebugUtilsMessageSeverityFlagsEXT MessageSeverity,
		vk::DebugUtilsMessageTypeFlagsEXT MessageType
	);

}