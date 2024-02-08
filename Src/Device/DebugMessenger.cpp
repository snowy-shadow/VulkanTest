#include "DebugMessenger.h"

#include <iostream>

namespace VT
{
	VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT, // MessageSeverity
		VkDebugUtilsMessageTypeFlagsEXT, // MessageType
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* // pUserData
	)
	{
		std::cout << "Message ID : " << pCallbackData->pMessageIdName << "\nMessage : " << pCallbackData->pMessage << "\n";
		return vk::False;
	}

	vk::DebugUtilsMessengerEXT createDebugMessenger(
		const vk::Instance& Instance,
		const vk::DispatchLoaderDynamic& DLD,
		vk::DebugUtilsMessageSeverityFlagsEXT MessageSeverity,
		vk::DebugUtilsMessageTypeFlagsEXT MessageType)
	{
		vk::DebugUtilsMessengerCreateInfoEXT CreateInfo
		{
			.messageSeverity = MessageSeverity,
			.messageType = MessageType,
			.pfnUserCallback = debugCallback
		};

		return Instance.createDebugUtilsMessengerEXT(CreateInfo, nullptr, DLD);
	}

}
