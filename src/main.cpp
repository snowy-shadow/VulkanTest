#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

// change -1.0 - 1.0 (OpenGL) -> 0.0 - 1.0 (vulkan)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// Load image
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

// hash for glm types
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stb_image.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

// Macros: EXIT_SUCESS, etc.
#include <cstdlib>

// data containers
#include <vector>
#include <array>
#include <unordered_map>

// strcmp for layer validation check
#include <cstring>

// Queue Family
#include <optional>
#include <set>

 // utin32_t
#include<cstdint>
// std::numeric_limits
#include <limits>
// std::clamp
#include <algorithm>

// Shader files
#include <fstream>

// Linear algebra
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

// Model loader
#include <tiny_obj_loader.h>

// Window properties
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string ModelPath = "../Resources/VikingRoom/viking_room.obj";
const std::string TexturePath = "../Resources/VikingRoom/viking_room.png";

const int MAX_FRAMES_IN_FLIGHT = 2;


const std::vector<const char*> ValidationLayers
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DeviceExtensions
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// NDEBUG is c++ macro (Not DEBUG)
#ifdef NDEBUG
const bool EnableValidationLayers{ false };

#else
const bool EnableValidationLyers{ true };

#endif

VkResult CreateDebugUtilsMessengerEXT
(
	VkInstance Instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
	auto Function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");

	if (Function != nullptr)
	{
		return Function(Instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT
(
	VkInstance Instance,
	VkDebugUtilsMessengerEXT DebugMessenger,
	const VkAllocationCallbacks* pAllocator
)
{
	auto Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");

	if (Function != nullptr)
	{
		Function(Instance, DebugMessenger, pAllocator);
	}
}

struct QueueFamilyIndices
{
	// take drawing commands
	std::optional<uint32_t> GraphicsFamily;
	// take presentation commands
	std::optional<uint32_t> PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

// some pipeline stages can be dynamic, but requires info to be passed in at draw time

// vertex data
struct Vertex
{
	glm::vec3 Pos;
	glm::vec2 TextureCoord;
	glm::vec3 Color;

	/*
	describes which rate to load data from memory throughout the vertices
	It specifies the number of bytes between data entries and whether to move to the next data entry after each vertex or after each instance
	*/
	static auto GetBindingDescription()
	{
		VkVertexInputBindingDescription BindingDescription{};
		// amount of seperate vertex data, its currently all in single vector so only 1 binding
		// specifies the index of the binding in the array of bindings
		BindingDescription.binding = 0;
		// number of bytes from one entry to the next
		BindingDescription.stride = sizeof(Vertex);
		/*
			VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
			VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
		*/
		BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return BindingDescription;
	}

	// how to handle vertex input
	static auto GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions{};
		// which binding the per-vertex data comes
		AttributeDescriptions[0].binding = 0;
		// references the location directive of the input in the vertex shader
		AttributeDescriptions[0].location = 0;
		/*
		type of data for attribute
			float: VK_FORMAT_R32_SFLOAT
			vec2: VK_FORMAT_R32G32_SFLOAT
			vec3: VK_FORMAT_R32G32B32_SFLOAT
			vec4: VK_FORMAT_R32G32B32A32_SFLOAT

		use the format where amount of color channels matches the number of components in the shader data type
			ivec2: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed integers
			uvec4: VK_FORMAT_R32G32B32A32_UINT, a 4-component vector of 32-bit unsigned integers
			double: VK_FORMAT_R64_SFLOAT, a double-precision (64-bit) float

		*/
		AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[0].offset = offsetof(Vertex, Pos);

		AttributeDescriptions[1].binding = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[1].offset = offsetof(Vertex, Color);

		AttributeDescriptions[2].binding = 0;
		AttributeDescriptions[2].location = 2;
		AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescriptions[2].offset = offsetof(Vertex, TextureCoord);

		return AttributeDescriptions;
	}

	bool operator == (const Vertex& Other) const
	{
		return Pos == Other.Pos && Color == Other.Color && TextureCoord == Other.TextureCoord;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& Vertex) const
		{
			return ((hash<glm::vec3>()(Vertex.Pos) ^
				(hash<glm::vec3>()(Vertex.Color) << 1)) >> 1) ^
				(hash<glm::vec2>()(Vertex.TextureCoord) << 1);
		}
	};
}

// this is binary compatible, allowing for memcpy
struct UniformBufferObject
{
	// use align to prevent weird behaviour
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};

class HelloTriangleApplication
{
public:
	void Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

private:
	// glfw Window
	GLFWwindow* Window;

	VkInstance Instance;
	VkDebugUtilsMessengerEXT DebugMessenger;
	// usage plateform agnostic, creation is not
	VkSurfaceKHR Surface;

	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkDevice Device;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;

	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	// pipeline
	VkRenderPass RenderPass;
	VkDescriptorSetLayout DescriptorSetLayout;
	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicsPipeline;

	// command center
	VkCommandPool CommandPool;
	std::vector<VkCommandBuffer> CommandBuffers;

	//synchronization
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphore;
	std::vector<VkFence> InFlightFences;
	uint32_t CurrentFrame = 0;

	bool FramebufferrResized = false;

	// vertex
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory;
	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;

	std::vector<VkBuffer> UniformBuffers;
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	std::vector<void*> UniformBuffersMapped;
	VkDescriptorPool DescriptorPool;
	std::vector<VkDescriptorSet> DescriptorSets;

	// Texture Image
	uint32_t MipLevels;
	VkImage TextureImage;
	VkDeviceMemory TextureImageMemory;
	// Texture image view
	VkImageView TextureImageView;
	VkSampler TextureSampler;

	// Depth image for depth testing
	VkImage DepthImage;
	VkDeviceMemory DepthImageMemory;
	VkImageView DepthImageView;

	// multi sample
	VkSampleCountFlagBits MSAA_Samples = VK_SAMPLE_COUNT_1_BIT;
	VkImage ColorImage;
	VkDeviceMemory ColorImageMemory;
	VkImageView ColorImageView;



	void InitWindow()
	{
		// init GLFW library
		glfwInit();

		// do not create opengl context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Window not resizeable
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// create Window
		Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // width, height, title, 
		// specify monitor to open Window, only for opengl
		// store user pointer inside Window
		glfwSetWindowUserPointer(Window, this);
		// detect resize
		glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
	}

	static void FramebufferResizeCallback(GLFWwindow* Window, int Width, int Height)
	{
		auto App = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(Window));
		App->FramebufferrResized = true;
	}

	void InitVulkan()
	{
		CreateVkInstance();
		SetupDebugMessenger();

		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();

		CreateCommandPool();
		CreateColorResources();
		CreateDepthResources();
		CreateFramebuffer();
		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();

		LoadModel();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffer();
		CreateDescriptorPool();
		CreateDescriptorSets();

		CreateCommandBuffer();
		CreateSyncObjects();

	}

	VkSampleCountFlagBits GetMaxUsageSampleCount()
	{
		VkPhysicalDeviceProperties PhysicalDeviceProperties;
		vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

		VkSampleCountFlags Count = PhysicalDeviceProperties.limits.framebufferColorSampleCounts &
			PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if (Count & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (Count & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (Count & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (Count & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (Count & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (Count & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	void LoadModel()
	{
		tinyobj::attrib_t Attribute;
		std::vector<tinyobj::shape_t> Shapes;
		std::vector <tinyobj::material_t> Materials;
		std::string Warn, Error;

		if(!tinyobj::LoadObj(&Attribute, &Shapes, &Materials, &Warn, &Error, ModelPath.c_str()))
		{
			throw std::runtime_error("Tiny_OBJ_Loader : " + Warn + Error);
		}

		std::unordered_map<Vertex, uint32_t> UniqueVertices{};

		for(const auto& Shape : Shapes)
		{
			for(const auto& Index : Shape.mesh.indices)
			{
				Vertex Vertex
				{
					.Pos
				{
					Attribute.vertices[3 * Index.vertex_index + 0],
					Attribute.vertices[3 * Index.vertex_index + 1],
					Attribute.vertices[3 * Index.vertex_index + 2]
					},

					.TextureCoord
				{
					Attribute.texcoords[2 * Index.texcoord_index + 0],
					1.f - Attribute.texcoords[2 * Index.texcoord_index + 1]
					},

					.Color {1.f, 1.f, 1.f}
				};

				// only push unique vertex
				if(!UniqueVertices.contains(Vertex))
				{
					UniqueVertices[Vertex] = static_cast<uint32_t>(Vertices.size());
					// emplace_back()
					Vertices.push_back(Vertex);
				}

				Indices.push_back(UniqueVertices[Vertex]);
			}
		}
	}

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& Candidates, VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for(VkFormat Format : Candidates)
		{
			/*
			 *The VkFormatProperties struct contains three fields
				linearTilingFeatures: Use cases that are supported with linear tiling
				optimalTilingFeatures: Use cases that are supported with optimal tiling
				bufferFeatures: Use cases that are supported for buffers
			 */
			VkFormatProperties Properties;
			vkGetPhysicalDeviceFormatProperties(PhysicalDevice, Format, &Properties);

			// require either or
			if (Tiling == VK_IMAGE_TILING_LINEAR && (Properties.linearTilingFeatures & Features) == Features) return Format;
			if (Tiling == VK_IMAGE_TILING_OPTIMAL && (Properties.optimalTilingFeatures & Features) == Features) return Format;
		}

		// if no match
		throw std::runtime_error("Failed to find supported format");
	}

	VkFormat FindDepthFormat()
	{
		return FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool HasStencilComponent(VkFormat Format)
	{
		return Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void CreateDepthResources()
	{
		VkFormat DepthFormat = FindDepthFormat();

		CreateImage(SwapChainExtent.width, SwapChainExtent.height, 1, MSAA_Samples,
			DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			DepthImage, DepthImageMemory);
		DepthImageView = CreateImageView(DepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}

	void CreateTextureImageView()
	{
		TextureImageView = CreateImageView(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, MipLevels);
	}

	void CreateTextureSampler()
	{
		VkPhysicalDeviceProperties PhysicalDeviceProperty{};
		vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperty);

		VkSamplerCreateInfo SamplerInfo
		{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,

			/*
			vectors UVW == XYZ
				VK_SAMPLER_ADDRESS_MODE_REPEAT: Repeat the texture when going beyond the image dimensions.
				VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: Like repeat, but inverts the coordinates to mirror the image when going beyond the dimensions.
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: Take the color of the edge closest to the coordinate beyond the image dimensions.
				VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: Like clamp to edge, but instead uses the edge opposite to the closest edge.
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: Return a solid color when sampling beyond the dimensions of the image.
			*/
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = PhysicalDeviceProperty.limits.maxSamplerAnisotropy,
			// which color is returned when sampling beyond the image
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};

		// comparison function, used for percentage closer filtering
		SamplerInfo.compareEnable = VK_TRUE;
		SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		SamplerInfo.mipLodBias = 0.f;
		SamplerInfo.minLod = 0.f;
		SamplerInfo.maxLod = static_cast<float>(MipLevels);

		if (vkCreateSampler(Device, &SamplerInfo, nullptr, &TextureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler");
		}
	}

	void CreateColorResources()
	{
		VkFormat ColorFormat = SwapChainImageFormat;

		CreateImage(SwapChainExtent.width, SwapChainExtent.height, 1, 
			MSAA_Samples,
			ColorFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			ColorImage, ColorImageMemory);

		ColorImageView = CreateImageView(ColorImage, ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	VkImageView CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags, uint32_t MipLevels)
	{
		VkImageViewCreateInfo ImageViewInfo
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = Image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = Format,
			.subresourceRange
			{
				.aspectMask = AspectFlags,
				.baseMipLevel = 0,
				.levelCount = MipLevels,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkImageView ImageView;
		if (vkCreateImageView(Device, &ImageViewInfo, nullptr, &ImageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		return ImageView;
	}

	void CreateImageViews()
	{
		SwapChainImageViews.resize(SwapChainImages.size());

		for (size_t i = 0; i < SwapChainImages.size(); i++)
		{
			//VkImageViewCreateInfo CreateVkImageViewInfo{};
			//CreateVkImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			//CreateVkImageViewInfo.image = SwapChainImages[i];
			//// how the image should be interpreted: 1D texture, 2D textures, 3D textures, cube maps
			//CreateVkImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			//CreateVkImageViewInfo.format = SwapChainImageFormat;

			//// swizzle color channels, used for monochrome texture etc. This is default mapping
			//CreateVkImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			//CreateVkImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			//CreateVkImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			//CreateVkImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			//// image purpose and what part of image should be accessed
			//CreateVkImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//CreateVkImageViewInfo.subresourceRange.baseMipLevel = 0;
			//CreateVkImageViewInfo.subresourceRange.levelCount = 1;
			//CreateVkImageViewInfo.subresourceRange.baseArrayLayer = 0;
			//CreateVkImageViewInfo.subresourceRange.layerCount = 1;

			//if (vkCreateImageView(Device, &CreateVkImageViewInfo, nullptr, &SwapChainImageViews[i]) != VK_SUCCESS)
			//{
			//    throw std::runtime_error("failed to create image views");
			//}

			SwapChainImageViews[i] = CreateImageView(SwapChainImages[i], SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	void CreateSwapChain()
	{
		SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDevice);

		VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
		VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
		VkExtent2D Extent2D = ChooseSwapExtent(SwapChainSupport.Capabilities);

		uint32_t ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;

		if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
		{
			ImageCount = SwapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR CreateSwapChainInfo{};
		CreateSwapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		CreateSwapChainInfo.surface = Surface;
		CreateSwapChainInfo.minImageCount = ImageCount;
		CreateSwapChainInfo.imageFormat = SurfaceFormat.format;
		CreateSwapChainInfo.imageColorSpace = SurfaceFormat.colorSpace;
		CreateSwapChainInfo.imageExtent = Extent2D;
		// amount of layer each image consists of
		CreateSwapChainInfo.imageArrayLayers = 1;
		// what kind of operation the image in swapchain is used for
		CreateSwapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices SwapChainIndicies = FindQueueFamilies(PhysicalDevice);
		uint32_t QueueFamilyIndices[]{ SwapChainIndicies.GraphicsFamily.value(), SwapChainIndicies.PresentFamily.value() };

		if (SwapChainIndicies.GraphicsFamily != SwapChainIndicies.PresentFamily)
		{
			// image can be transfered without explicit transfers
			CreateSwapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			CreateSwapChainInfo.queueFamilyIndexCount = 2;
			CreateSwapChainInfo.pQueueFamilyIndices = QueueFamilyIndices;
		}
		else
		{
			// owned by one queue family at a time, transfer must be explicit, best performance
			CreateSwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			// optional
			CreateSwapChainInfo.queueFamilyIndexCount = 0;
			// optional
			CreateSwapChainInfo.pQueueFamilyIndices = nullptr;
		}
		// specify apply no image transformation
		CreateSwapChainInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
		// alpha channel
		CreateSwapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		CreateSwapChainInfo.presentMode = PresentMode;
		// don't care about color pixel that are obscured
		CreateSwapChainInfo.clipped = VK_TRUE;
		// will not inherit from a previous created chain
		CreateSwapChainInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(Device, &CreateSwapChainInfo, nullptr, &SwapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to created swap chain");
		}

		// retrieve handle to end of SwapChain function
		vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, nullptr);
		SwapChainImages.resize(ImageCount);
		vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, SwapChainImages.data());

		SwapChainImageFormat = SurfaceFormat.format;
		SwapChainExtent = Extent2D;
	}

	void RecreateSwapChain()
	{
		int Width = 0, Height = 0;
		glfwGetFramebufferSize(Window, &Width, &Height);
		while (Width == 0 || Height == 0)
		{
			glfwGetFramebufferSize(Window, &Width, &Height);
			glfwWaitEvents();
		}

		// don't want to touch resources that might still be processing
		// alternatively, can use SwapchainCreateInfo.oldSwapChain to allocate new Swapchain while processing
		vkDeviceWaitIdle(Device);
		SwapChainCleanUp();
		CreateSwapChain();
		CreateImageViews();
		CreateColorResources();
		CreateDepthResources();
		CreateFramebuffer();
	}

	void CreateSurface()
	{
		if (glfwCreateWindowSurface(Instance, Window, nullptr, &Surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create Window surface");
		}
	}

	void CreateLogicalDevice()
	{
		QueueFamilyIndices QueueIndices = FindQueueFamilies(PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
		std::set<uint32_t> UniqueQueueFamilies{ QueueIndices.GraphicsFamily.value(), QueueIndices.PresentFamily.value() };

		// command buffer execution priority 0.0 - 1.0
		float QueuePriority = 1.f;
		for (uint32_t QueueFamily : UniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo QueueCreateDeviceInfo{};
			QueueCreateDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueCreateDeviceInfo.queueFamilyIndex = QueueFamily;
			QueueCreateDeviceInfo.queueCount = 1;
			QueueCreateDeviceInfo.pQueuePriorities = &QueuePriority;
			QueueCreateInfos.push_back(QueueCreateDeviceInfo);
		}

		VkPhysicalDeviceFeatures DeviceFeatures
		{
			.samplerAnisotropy = VK_TRUE
		};
		DeviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo DeviceCreateInfo{};
		DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
		DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());;
		DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
		DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

		if (EnableValidationLyers)
		{
			DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			DeviceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
		}
		else { DeviceCreateInfo.enabledLayerCount = 0; }

		// Device to interface, Queue + useage info, optional allocation callback pointer, pointer to variable to store logical device handle
		if (vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &Device) != VK_SUCCESS)
		{
			throw std::runtime_error("faield to create local device");
		}

		vkGetDeviceQueue(Device, QueueIndices.GraphicsFamily.value(), 0, &GraphicsQueue);
		vkGetDeviceQueue(Device, QueueIndices.PresentFamily.value(), 0, &PresentQueue);
	}

	void PickPhysicalDevice()
	{
		uint32_t DeviceCount = 0;
		// Get num of capable devices
		vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);

		if (DeviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> Devices(DeviceCount);
		// Get list of devices
		vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

		for (const auto& Device : Devices)
		{
			if (IsDeviceSuitable(Device))
			{
				PhysicalDevice = Device;
				MSAA_Samples = GetMaxUsageSampleCount();
				break;
			}
		}

		if (PhysicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

	}

	void CreateVkInstance()
	{
		if (EnableValidationLyers && !CheckValidationLayerSupport())
		{
			// this check prevents VK_ERROR_LAYER_NOT_PRESENT
			throw std::runtime_error("validation layers requested, but not available");
		}

		// AppInfo struct
		VkApplicationInfo AppInfo{};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pApplicationName = "Hellow Triangle";

		// VK_MAKE_VERSION deprecate, use VK_MAKE_API_VERSION. Variant = 0 for Vulkan API
		AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.pEngineName = "No Engine";
		AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.apiVersion = VK_API_VERSION_1_0;

		// ----------------------------------------------------------------

		VkInstanceCreateInfo CreateVkInfo{};
		CreateVkInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateVkInfo.pApplicationInfo = &AppInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		// VK need extension to interface with Window, GLFW can return extension(s) VK needs
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		CreateVkInfo.enabledExtensionCount = glfwExtensionCount;

		// passed to VK
		CreateVkInfo.ppEnabledExtensionNames = glfwExtensions;

		// extension debugger messages
		const std::vector<const char*> Extensions = GetRequiredExtensions();
		CreateVkInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
		CreateVkInfo.ppEnabledExtensionNames = Extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT DebugCreateVkInfo{};
		// global validation layers to enable
		if (EnableValidationLyers)
		{
			CreateVkInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			CreateVkInfo.ppEnabledLayerNames = ValidationLayers.data();

			PopulateDebugMessengerCreateInfo(DebugCreateVkInfo);
			CreateVkInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateVkInfo;
		}
		else
		{
			CreateVkInfo.enabledLayerCount = 0;

			CreateVkInfo.pNext = nullptr;
		}

		/*
		replaced by GetRequiredExtensions()
			// in case of VK_ERROR_EXTENSION_NOT_PRESENT, get list of current extensions
			uint32_t InstanceExtensionCount = 0;

			// request # of extensions. Take Ptr to Var that stores # of extensions + array of VkExtensionProperties(details of extension)
			// first parameter filter extension by specific validation layer
			vkEnumerateInstanceExtensionProperties(nullptr, &InstanceExtensionCount, nullptr);

			std::vector<VkExtensionProperties> InstanceExtensions(InstanceExtensionCount);

			vkEnumerateInstanceExtensionProperties(nullptr, &InstanceExtensionCount, InstanceExtensions.data());
		*/


		VkResult Result = vkCreateInstance(&CreateVkInfo, nullptr, &Instance);  /* Object creation in VK
																				pointer to struct with creation info
																				pointer to custom allocator callbacks
																				pointer to variable that stores handle to new obj created
																				*/

																				// almost all VK functions return value of vkResult: VK_SUCCESS or an error code
		if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}


	}

	void MainLoop()
	{
		// while Window == open
		while (!glfwWindowShouldClose(Window))
		{
			glfwPollEvents();
			DrawFrame();
		}

		// wait for all operations to finish, can also use vkQueueWaitIdle
		vkDeviceWaitIdle(Device);
	}

	void Cleanup()
	{
		SwapChainCleanUp();

		// texture
		vkDestroySampler(Device, TextureSampler, nullptr);
		vkDestroyImageView(Device, TextureImageView, nullptr);
		vkDestroyImage(Device, TextureImage, nullptr);
		vkFreeMemory(Device, TextureImageMemory, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyBuffer(Device, UniformBuffers[i], nullptr);
			vkFreeMemory(Device, UniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(Device, DescriptorSetLayout, nullptr);

		vkDestroyPipeline(Device, GraphicsPipeline, nullptr);

		vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);

		vkDestroyRenderPass(Device, RenderPass, nullptr);

		vkDestroyBuffer(Device, IndexBuffer, nullptr);
		vkFreeMemory(Device, IndexBufferMemory, nullptr);

		vkDestroyBuffer(Device, VertexBuffer, nullptr);
		vkFreeMemory(Device, VertexBufferMemory, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(Device, RenderFinishedSemaphore[i], nullptr);
			vkDestroyFence(Device, InFlightFences[i], nullptr);
		}

		// also frees command buffer automatically when pool is freed
		vkDestroyCommandPool(Device, CommandPool, nullptr);

		vkDestroyDevice(Device, nullptr);

		if (EnableValidationLyers)
		{
			DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
		}

		// destroy before instance
		vkDestroySurfaceKHR(Instance, Surface, nullptr);

		// VKInstance should only be destroyed right before program exit
		// optional allocator callback
		vkDestroyInstance(Instance, nullptr);

		glfwDestroyWindow(Window);

		glfwTerminate();
	}

	void SwapChainCleanUp()
	{
		vkDestroyImageView(Device, ColorImageView, nullptr);
		vkDestroyImage(Device, ColorImage, nullptr);
		vkFreeMemory(Device, ColorImageMemory, nullptr);

		vkDestroyImageView(Device, DepthImageView, nullptr);
		vkDestroyImage(Device, DepthImage, nullptr);
		vkFreeMemory(Device, DepthImageMemory, nullptr);

		// make sure old versions are removed
		for (const auto& Framebuffer : SwapChainFramebuffers)
		{
			vkDestroyFramebuffer(Device, Framebuffer, nullptr);
		}

		for (const auto& ImageView : SwapChainImageViews)
		{
			vkDestroyImageView(Device, ImageView, nullptr);
		}

		vkDestroySwapchainKHR(Device, SwapChain, nullptr);
	}

	bool CheckValidationLayerSupport()
	{
		// same call as vkEnumerateInstanceExtensionProperties(), see below
		uint32_t LayerCount{};
		// get amount of layers
		vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

		std::vector<VkLayerProperties> AvailableLayers(LayerCount);
		// retrieve list of layers
		vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

		// check if layers are availiable
		for (const char* LayerName : ValidationLayers)
		{
			bool LayerFound = false;

			for (const auto& LayerProperties : AvailableLayers)
			{
				if (strcmp(LayerName, LayerProperties.layerName) == 0)
				{
					LayerFound = true;
					break;
				}
			}

			if (!LayerFound)
			{
				return false;
			}
		}

		return true;
	}

	// explicit debug message output, not all are fatal error
	const std::vector<const char*> GetRequiredExtensions()
	{
		// return required list of extensions based on if validation layer are required
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		// GLFW extension are always required
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> Extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (EnableValidationLyers)
		{
			Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return Extensions;
	}

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& CreateDebugInfo)
	{
		// see notes in SetupDebugMessenger()
		CreateDebugInfo = {};

		CreateDebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

		// messages severity displayed
		CreateDebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		// message type displayed
		CreateDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		CreateDebugInfo.pfnUserCallback = DebugCallback;
	}

	void SetupDebugMessenger()
	{
		if (!EnableValidationLyers)
		{
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT CreateDebuggerInfo{};
		PopulateDebugMessengerCreateInfo(CreateDebuggerInfo);

		/*
			// specify all types of severities callback that are wanted
			CreateDebuggerInfo.messageSeverity

			// filter types of message callback is notified
			CreateDebuggerInfo.messageType

			// pointer to callback function
			CreateDebuggerInfo.pfnUserCallback

			// optional
			CreateDebuggerInfo.pUserData
		*/

		if (CreateDebugUtilsMessengerEXT(Instance, &CreateDebuggerInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	// PFN_vkDebugUtilsMessengerCallbackEXT prototype
	// VKAPI_ATT, VKAPI_CALL Vulkan signature for call
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		/*
		severity of the message
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but very likely a bug in your application
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes

			Message to show
			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){}
		*/
		VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,

		/*
		Messge types
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification or performance
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the specification or indicates a possible mistake
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal use of Vulkan
		*/
		VkDebugUtilsMessageTypeFlagsEXT MessageType,

		/*
		contains detials of the message
			pMessage: The debug message as a null-terminated string
			pObjects: Array of Vulkan object handles related to the message
			objectCount: Number of objects in array
		*/
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,

		/*
		specified during setup of callback that allows for custom data to be passed in
		*/
		void* pUserData
	)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;

	}

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device)
	{
		QueueFamilyIndices QueueIndices;

		uint32_t QueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

		int GPUList = 0;
		for (const auto& QueueFamily : QueueFamilies)
		{
			if (QueueIndices.IsComplete()) { break; }

			VkBool32 PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(Device, GPUList, Surface, &PresentSupport);

			if (PresentSupport)
			{
				QueueIndices.PresentFamily = GPUList;
			}

			if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueIndices.GraphicsFamily = GPUList;
			}
			GPUList++;
		}

		return QueueIndices;
	}

	bool IsDeviceSuitable(VkPhysicalDevice Device)
	{
		/*
			// device name, type, supported vulkan version, etc
			VkPhysicalDeviceProperties DeviceProperties;
			vkGetPhysicalDeviceProperties(Device, &DeviceProperties);

			// support for optional features, texture compression, 64 bit float, multi viewport rendering (VR), etc
			VkPhysicalDeviceFeatures DeviceFeatures;
			vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

			can also rank gpus by feature and select best ones
		*/

		QueueFamilyIndices Indices = FindQueueFamilies(Device);

		VkPhysicalDeviceFeatures DeviceFeatures;
		vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);

		bool ExtensionsSupported = CheckDeviceExtensionSupport(Device);

		bool SwapChainAdequate = false;
		if (ExtensionsSupported)
		{
			SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(Device);
			SwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
		}

		return Indices.GraphicsFamily.has_value() && ExtensionsSupported && SwapChainAdequate && DeviceFeatures.samplerAnisotropy;
	}

	bool CheckDeviceExtensionSupport(VkPhysicalDevice Device)
	{
		uint32_t ExtensionCount;
		vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, nullptr);

		std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
		vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, AvailableExtensions.data());

		std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

		for (const auto& Extension : AvailableExtensions)
		{
			RequiredExtensions.erase(Extension.extensionName);
		}

		return RequiredExtensions.empty();
	}

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device)
	{
		SwapChainSupportDetails Details;

		// take Device and Surface into account when determing capability
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);

		uint32_t FormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);

		if (FormatCount != 0)
		{
			Details.Formats.resize(FormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Details.Formats.data());
		}

		uint32_t PresentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr);

		if (PresentModeCount != 0)
		{
			Details.PresentModes.resize(PresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes.data());
		}

		return Details;
	}

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
	{
		// format: specifies color channels and type, Colorspace: SGRB...etc
		for (const auto& AvailableFormat : AvailableFormats)
		{
			if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				AvailableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return AvailableFormat;
			}
		}

		return AvailableFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
	{
		/*
		VK_PRESENT_MODE_IMMEDIATE_KHR
		VK_PRESENT_MODE_FIFO_KHR <- guaranteed availability
		VK_PRESENT_MODE_FIFO_RELAXED_KHR
		VK_PRESENT_MODE_MAILBOX_KHR
		*/
		for (const auto& AvailablePresentMode : AvailablePresentModes)
		{
			if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return AvailablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& SurfaceCapabilities)
	{
		// resolution of the swap chain images
		if (SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return SurfaceCapabilities.currentExtent;
		}

		else
		{
			int Width{}, Height{};
			glfwGetFramebufferSize(Window, &Width, &Height);

			VkExtent2D ActualExtent
			{
				static_cast<uint32_t>(Width),
				static_cast<uint32_t>(Height)
			};

			ActualExtent.width = std::clamp(ActualExtent.width,
				SurfaceCapabilities.minImageExtent.width,
				SurfaceCapabilities.maxImageExtent.width);

			ActualExtent.height = std::clamp(ActualExtent.height,
				SurfaceCapabilities.minImageExtent.height,
				SurfaceCapabilities.maxImageExtent.height);

			return ActualExtent;
		}
	}

	void CreateDescriptorSetLayout()
	{
		// def UBO
		VkDescriptorSetLayoutBinding UBO_LayoutBinding{};
		// binding used
		UBO_LayoutBinding.binding = 0;
		// type
		UBO_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// shader var can represent an array of UBO, specify number of values in array
		UBO_LayoutBinding.descriptorCount = 1;
		// which shader stage the descriptor is being referenced
		UBO_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		// optional
		UBO_LayoutBinding.pImmutableSamplers = nullptr;

		// def Sampler
		VkDescriptorSetLayoutBinding SamplerLayoutBinding
		{
			.binding = 1,
			.descriptorCount = 1,
		};

		SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		SamplerLayoutBinding.pImmutableSamplers = nullptr;
		SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> Bindings = { UBO_LayoutBinding, SamplerLayoutBinding };

		// create descriptor layout
		VkDescriptorSetLayoutCreateInfo LayoutInfo{};
		LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
		LayoutInfo.pBindings = Bindings.data();

		if (vkCreateDescriptorSetLayout(Device, &LayoutInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}

	}

	void CreateGraphicsPipeline()
	{
		auto VertShaderCode = ReadFile("../Resources/vert.spv");
		auto FragShaderCode = ReadFile("../Resources/frag.spv");

		// can destroy shader module as soon as pipeline creation is finished, so no need to be member variable
		VkShaderModule VertShaderModule = CreateShaderModule(VertShaderCode);
		VkShaderModule FragShaderModule = CreateShaderModule(FragShaderCode);

		VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
		VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// which pipeline shader is used, enum associated for each stage
		VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		// which module used
		VertShaderStageInfo.module = VertShaderModule;
		// function to invoke, entrypoint
		VertShaderStageInfo.pName = "main";

		// same process for fragment shader
		VkPipelineShaderStageCreateInfo FragShaderStageInfo{};
		FragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		FragShaderStageInfo.module = FragShaderModule;
		FragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo ShaderStages[]{ VertShaderStageInfo, FragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
		VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto BindingDescription = Vertex::GetBindingDescription();
		auto AttributeDescriptions = Vertex::GetAttributeDescriptions();

		VertexInputInfo.vertexBindingDescriptionCount = 1;
		// point to struct of info that describes type of vertex (per vertex, per instance)
		VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
		VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescriptions.size());
		// struct containing types of attribute
		VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();

		// VkPipelineInputAssemblyStateCreateInfo: what kinds of geometry draw from vertices, if primitive restart enabled
		/*
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle
		*/

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo{};
		InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// can reuse vertices and perform custom optimizations when true
		InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo ViewportStateInfo{};
		ViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportStateInfo.viewportCount = 1;
		ViewportStateInfo.scissorCount = 1;

		// rasterizer performs depth testing, face culling, scissor test, wireframe rendering
		VkPipelineRasterizationStateCreateInfo RasterizerInfo{};
		RasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// fragments beyond near and far planes clamped or discarded
		RasterizerInfo.depthClampEnable = VK_FALSE;
		// geometry never pass rasterizer stage if true
		RasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		/*
		determines how fragments are generated for geometry

			VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
			VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
			VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
		*/
		RasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		// line width in terms of number of fragments
		RasterizerInfo.lineWidth = 1.0f;
		// type of face culling: disable culling, cull the front faces, cull the back faces or both
		RasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		// vertex order for faces considered front: clockwise or counter clockwise
		RasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		RasterizerInfo.depthBiasEnable = VK_FALSE;
		// optionals, can alter depth vlaue by adding constant value or bias based on fragment's slope
		RasterizerInfo.depthBiasConstantFactor = 0.0f;
		RasterizerInfo.depthBiasClamp = 0.0f;
		RasterizerInfo.depthBiasSlopeFactor = 0.0f;

		// multisampling, one way to antialiasing, combines multiple polygon that rasterizes to the same pixel
		VkPipelineMultisampleStateCreateInfo MultisamplingInfo{};
		MultisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultisamplingInfo.sampleShadingEnable = VK_TRUE;
		MultisamplingInfo.rasterizationSamples = MSAA_Samples;
		// optionals
		MultisamplingInfo.minSampleShading = 0.2f;
		MultisamplingInfo.pSampleMask = nullptr;
		MultisamplingInfo.alphaToCoverageEnable = VK_FALSE;
		MultisamplingInfo.alphaToOneEnable = VK_FALSE;

		/*
		color blending, 2 ways:
			Mix the old and new value to produce a final color
			Combine the oldand new value using a bitwise operation
		*/
		// 2 different structs for types of color blending, this is per frambuffer
		VkPipelineColorBlendAttachmentState ColorblendAttachmentInfo{};

		ColorblendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		ColorblendAttachmentInfo.blendEnable = VK_FALSE;
		// optionals
		ColorblendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorblendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorblendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
		ColorblendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorblendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorblendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;
		/*
		per framebuffer operation pseudo code:
		if (blendEnable)
		{
			finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
			finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		}
		else
		{
			finalColor = newColor;
		}

		finalColor = finalColor & colorWriteMask;
		*/

		VkPipelineColorBlendStateCreateInfo ColorBlending{};
		ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		// true for bitwise combination, this automatically disable the other method
		ColorBlending.logicOpEnable = VK_FALSE;
		// optional
		ColorBlending.logicOp = VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount = 1;
		ColorBlending.pAttachments = &ColorblendAttachmentInfo;
		// optionals
		ColorBlending.blendConstants[0] = 0.0f;
		ColorBlending.blendConstants[1] = 0.0f;
		ColorBlending.blendConstants[2] = 0.0f;
		ColorBlending.blendConstants[3] = 0.0f;

		// pipeline layout
		std::vector<VkDynamicState> DynamicStates
		{
			VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo DynamicState{};
		DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
		DynamicState.pDynamicStates = DynamicStates.data();

		// Depth and stencil state
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo{};
		DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		// enable depth test
		DepthStencilInfo.depthTestEnable = VK_TRUE;
		// if new fragments should be recorded
		DepthStencilInfo.depthWriteEnable = VK_TRUE;
		// what kind of test
		DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		// specific testing boundary
		DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		DepthStencilInfo.minDepthBounds = 0.0f;
		DepthStencilInfo.maxDepthBounds = 1.0f;
		// stencil buffer operations
		DepthStencilInfo.stencilTestEnable = VK_FALSE;
		DepthStencilInfo.front = {};
		DepthStencilInfo.back = {};

		VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// optionals
		PipelineLayoutInfo.setLayoutCount = 1;
		PipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
		// another way of passing dynamic values to shaders
		PipelineLayoutInfo.pushConstantRangeCount = 0;
		PipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(Device, &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}

		VkGraphicsPipelineCreateInfo PipelineInfo{};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.stageCount = 2;
		PipelineInfo.pStages = ShaderStages;
		PipelineInfo.pVertexInputState = &VertexInputInfo;
		PipelineInfo.pInputAssemblyState = &InputAssemblyInfo;
		PipelineInfo.pViewportState = &ViewportStateInfo;
		PipelineInfo.pRasterizationState = &RasterizerInfo;
		PipelineInfo.pMultisampleState = &MultisamplingInfo;
		// optional
		PipelineInfo.pDepthStencilState = &DepthStencilInfo;
		PipelineInfo.pColorBlendState = &ColorBlending;
		PipelineInfo.pDynamicState = &DynamicState;
		// fixed function stages
		PipelineInfo.layout = PipelineLayout;
		PipelineInfo.renderPass = RenderPass;
		// index of subpass used
		PipelineInfo.subpass = 0;
		// optionals
		// can derive new pipeline from old
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		PipelineInfo.basePipelineIndex = -1;

		// can create multiple pipelines at once, VK_NULL_HANDLE = pipeline cache obj for data reuse
		if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}

		// Shader module no longer needed
		vkDestroyShaderModule(Device, VertShaderModule, nullptr);
		vkDestroyShaderModule(Device, FragShaderModule, nullptr);
	}

	VkShaderModule CreateShaderModule(const std::vector<char>& Shader)
	{
		// wrapper around shader byte code
		VkShaderModuleCreateInfo ShaderInfo{};
		ShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ShaderInfo.codeSize = Shader.size();
		ShaderInfo.pCode = reinterpret_cast<const uint32_t*>(Shader.data());

		VkShaderModule ShaderModule;
		if (vkCreateShaderModule(Device, &ShaderInfo, nullptr, &ShaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module");
		}

		return ShaderModule;
	}

	static std::vector<char> ReadFile(const std::string& FileName)
	{
		std::ifstream File(FileName, std::ios::ate | std::ios::binary);

		if (!File.is_open())
		{
			throw std::runtime_error("failed to open file");
		}

		size_t FileSize = (size_t)File.tellg();
		std::vector<char> Buffer(FileSize);

		File.seekg(0);
		File.read(Buffer.data(), FileSize);

		File.close();

		return Buffer;
	}

	void CreateRenderPass()
	{
		// framebuffer attachments used while rendering
		VkAttachmentDescription ColorAttachment{};
		ColorAttachment.format = SwapChainImageFormat;
		// not multisample
		ColorAttachment.samples = MSAA_Samples;
		/*
		color and depth data
		data in the attachment before and after rendering
			VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
			VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
			VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
		*/
		ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		/*
			VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
			VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
		*/
		ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// stencil data
		ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// not using stencil buffer
		ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		/*
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
		*/

		// rendering subpass
		VkAttachmentReference ColorAttachmentRef{};
		// attachment to reference by index
		ColorAttachmentRef.attachment = 0;
		// which layout the attachment to have during subpass, we want to use it as a color buffer
		ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// multisampled images cannot be presented directly, need to resolve to regular image
		VkAttachmentDescription ColorAttachmentResolve{};
		ColorAttachmentResolve.format = SwapChainImageFormat;
		ColorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference ColorAttachmentResolveReference{};
		ColorAttachmentResolveReference.attachment = 2;
		ColorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// depth buffer
		VkAttachmentDescription DepthAttachment{};
		DepthAttachment.format = FindDepthFormat();
		DepthAttachment.samples = MSAA_Samples;
		DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// will not need the data after finished drawing
		DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// don't care about previous depth info
		DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference DepthAttachmentReference{};
		DepthAttachmentReference.attachment = 1;
		DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription Subpass{};
		// type of subpass
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = 1;
		/*
		other types of attachment
			pInputAttachments: Attachments that are read from a shader
			pResolveAttachments: Attachments used for multisampling color attachments
			pDepthStencilAttachment: Attachment for depth and stencil data
			pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be preserved
		*/
		Subpass.pColorAttachments = &ColorAttachmentRef;
		Subpass.pResolveAttachments = &ColorAttachmentResolveReference;
		Subpass.pDepthStencilAttachment = &DepthAttachmentReference;

		VkSubpassDependency SubpassDependency{};
		// indices of dependency
		// VK_SUBPASS_EXTERNAL refers to the implicit subpass before or after depending if its in srcSubpass or dstSubpass
		SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		// the dependent subpass, dstSubpass must always be higher than srcSubpass
		SubpassDependency.dstSubpass = 0;
		// which operation to wait on
		SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		// the stages where the operation waiting on occures
		SubpassDependency.srcAccessMask = 0;
		// prevents transition from happening until necessary / allowed
		SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> Attachments{ColorAttachment, DepthAttachment, ColorAttachmentResolve};

		VkRenderPassCreateInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
		RenderPassInfo.pAttachments = Attachments.data();
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &Subpass;
		// array of dependencies
		RenderPassInfo.dependencyCount = 1;
		RenderPassInfo.pDependencies = &SubpassDependency;

		if (vkCreateRenderPass(Device, &RenderPassInfo, nullptr, &RenderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create renderpass");
		}
	}

	void CreateFramebuffer()
	{
		// must be compatible with the type of image within the imageview
		SwapChainFramebuffers.resize(SwapChainImageViews.size());
		for (size_t i = 0; i < SwapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 3> Attachments
			{
				ColorImageView,
				DepthImageView,
				SwapChainImageViews[i]
			};

			VkFramebufferCreateInfo FramebufferInfo{};
			FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferInfo.renderPass = RenderPass;
			FramebufferInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
			FramebufferInfo.pAttachments = Attachments.data();
			FramebufferInfo.width = SwapChainExtent.width;
			FramebufferInfo.height = SwapChainExtent.height;
			FramebufferInfo.layers = 1;

			if (vkCreateFramebuffer(Device, &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer");
			}
		}
	}

	void CreateCommandPool()
	{
		QueueFamilyIndices QueueMyFamilyIndices = FindQueueFamilies(PhysicalDevice);

		VkCommandPoolCreateInfo PoolInfo{};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		/*
		two possible flags
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded with new commands very often (may change memory allocation behavior)
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
		*/
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		PoolInfo.queueFamilyIndex = QueueMyFamilyIndices.GraphicsFamily.value();

		if (vkCreateCommandPool(Device, &PoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool");
		}

	}

	void CreateCommandBuffer()
	{
		CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = CommandPool;
		/*
			VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
			VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
		*/
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		// number to buffer to allocate
		AllocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

		// specify the command pool
		if (vkAllocateCommandBuffers(Device, &AllocInfo, CommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}

	}

	void CreateBuffer(VkDeviceSize DeviceSize, VkBufferUsageFlags BufferUsage, VkMemoryPropertyFlags Properties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory)
	{
		// Buffer creation helper

		VkBufferCreateInfo BufferInfo{};
		BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		// size in bytes
		BufferInfo.size = DeviceSize;
		// indicates purpose of buffer
		BufferInfo.usage = BufferUsage;
		// buffer can be owned by specific queue family or shared like swap chain
		BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(Device, &BufferInfo, nullptr, &Buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer");
		}
		/*
		assign memory to vertex buffer
		three fields
			size: The size of the required amount of memory in bytes, may differ from bufferInfo.size.
			alignment: The offset in bytes where the buffer begins in the allocated region of memory, depends on bufferInfo.usage and bufferInfo.flags.
			memoryTypeBits: Bit field of the memory types that are suitable for the buffer.
		*/
		VkMemoryRequirements MemRequirements;
		vkGetBufferMemoryRequirements(Device, Buffer, &MemRequirements);

		VkMemoryAllocateInfo MemAllocInfo{};
		MemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		MemAllocInfo.allocationSize = MemRequirements.size;
		MemAllocInfo.memoryTypeIndex = FindMemoryType(MemRequirements.memoryTypeBits, Properties);

		if (vkAllocateMemory(Device, &MemAllocInfo, nullptr, &BufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memeory");
		}

		vkBindBufferMemory(Device, Buffer, BufferMemory, 0/*offset within mem*/);
	}

	void CreateVertexBuffer()
	{
		VkDeviceSize BufferSize = sizeof(Vertices[0]) * Vertices.size();

		// create 1 host visible buffer as temp buffer, 1 device local as actual buffer
		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;

		/*
		Temp host visible buffer
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory transfer operation.
			VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as destination in a memory transfer operation.
		 */
		CreateBuffer(BufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			StagingBuffer,
			StagingBufferMemory);


		void* Data;

		vkMapMemory(Device, StagingBufferMemory, 0/*offset*/, BufferSize/*size of mem*/, 0, &Data);
		/*
		if mapped data not present or visible
			Use a memory heap that is host coherent, indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			Call vkFlushMappedMemoryRanges after writing to the mapped memory, and call vkInvalidateMappedMemoryRanges before reading from the mapped memory
		*/
		memcpy(Data, Vertices.data(), (size_t)BufferSize);
		vkUnmapMemory(Device, StagingBufferMemory);

		// device local
		CreateBuffer(BufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VertexBuffer, VertexBufferMemory);

		CopyBuffer(StagingBuffer, VertexBuffer, BufferSize);

		// clean up
		vkDestroyBuffer(Device, StagingBuffer, nullptr);
		vkFreeMemory(Device, StagingBufferMemory, nullptr);
	}

	void CreateIndexBuffer()
	{
		VkDeviceSize BufferSize = sizeof(Indices[0]) * Indices.size();

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;

		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			StagingBuffer,
			StagingBufferMemory);

		void* Data;
		vkMapMemory(Device, StagingBufferMemory, 0, BufferSize, 0, &Data);
		memcpy(Data, Indices.data(), (size_t)BufferSize);
		vkUnmapMemory(Device, StagingBufferMemory);

		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			IndexBuffer,
			IndexBufferMemory);

		CopyBuffer(StagingBuffer, IndexBuffer, BufferSize);

		vkDestroyBuffer(Device, StagingBuffer, nullptr);
		vkFreeMemory(Device, StagingBufferMemory, nullptr);


	}

	void CreateUniformBuffer()
	{
		VkDeviceSize BufferSize = sizeof(UniformBufferObject);

		UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			CreateBuffer(BufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				UniformBuffers[i],
				UniformBuffersMemory[i]);

			// persistent mapping
			vkMapMemory(Device, UniformBuffersMemory[i], 0, BufferSize, 0, &UniformBuffersMapped[i]);
		}
	}

	void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
	{
		VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

		VkBufferCopy CopyRegion{};
		// optionals
		CopyRegion.srcOffset = 0;
		CopyRegion.dstOffset = 0;

		CopyRegion.size = Size;
		vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

		EndSingleTimeCommands(CommandBuffer);
	}

	uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
	{
		/*
		2 arrays:
			memory type: VRAM, swap in RAM, etc
			memory heap: where the mem comes from
		*/
		VkPhysicalDeviceMemoryProperties MemProperties;
		vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemProperties);

		for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
		{
			if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type");
	}

	// writes the command to the buffer for execution
	void RecordCommandBuffer(VkCommandBuffer CommandBuffer, uint32_t ImageIndex)
	{
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// optionals
		/*
		flag indicates how is the buffer used
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
		*/
		BeginInfo.flags = 0;
		// for secondary command buffers, which state to inherit from calling primary command buffer
		BeginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		// attachments to bind
		RenderPassInfo.renderPass = RenderPass;
		// framebuffer for each swapchain image, and bind framebuffer to the image
		RenderPassInfo.framebuffer = SwapChainFramebuffers[ImageIndex];
		// size of render area, defines where shader loads and stores. Pixels ouside the region will have undefined value -> match size of attchments for best performance
		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = SwapChainExtent;
		/* define clear values to use of VK_ATTACHMENT_LOAD_OP_CLEAR, currently defined as black with 100% opacity
		 * order of clear values should be identical to the order of attachments
		 */
		std::array<VkClearValue, 2> ClearColors{};
		ClearColors[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		// 0.0 is nearest, 1.0 is the furtherest
		ClearColors[1].depthStencil = {1.0f, 0};

		RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearColors.size());
		RenderPassInfo.pClearValues = ClearColors.data();

		// all functions that record command have "vkCmd" prefix, returns void -> no error handling until finished recording
		/*
		where to command to, details of the render pass, how the drawing commands will be provided
		Drawing commands:
			VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.
			VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from secondary command buffers.
		*/
		vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// where to command to, pipeline type: graphics or compute, graphics pipeline operations
		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

		// viewport and scissor can be static or dynamic, need to enable dynamic state for pipeline to become dynamic
		// can have multiple viewport and scissor wihtin single command buffer if dynamic
		VkViewport Viewport{};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = static_cast<float>(SwapChainExtent.width);
		Viewport.height = static_cast<float>(SwapChainExtent.height);
		// depth [0.f - 1.f]
		// min depth can be larger than max depth
		Viewport.minDepth = 0.f;
		Viewport.maxDepth = 1.f;
		vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);

		// any pixel outside of scissor rectangle WILL BE DISCRADED by the rasterizer even if viewport includes it
		VkRect2D Scissor{};
		Scissor.offset = { 0,0 };
		Scissor.extent = SwapChainExtent;
		vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

		/*
			for static inside pipeline creation
			VkPipelineViewportStateCreateInfo ViewportStateInfo{};
			ViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			ViewportStateInfo.viewportCount = 1;
			ViewportStateInfo.pViewports = &Viewport;
			ViewportStateInfo.scissorCount = 1;
			ViewportStateInfo.pScissors = &Scissor;
		*/

		/*
			where to command to
			vertexCount: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
			instanceCount: Used for instanced rendering, use 1 if you're not doing that.
			firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
			firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
		*/
		// vkCmdDraw(CommandBuffer, 3, 1, 0, 0);

		VkBuffer Vertexbuffers[] = { VertexBuffer };
		VkDeviceSize Offsets[] = { 0 };
		// cmdbuffer, offset, number of bindings, array of vertex buffers to bind, byte offset to start reading vertex data
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Vertexbuffers, Offsets);
		vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

		/*
			where descriptor set binds to
			layout descriptor set based on
			index of first descriptor set
			number of sets to bind
			array of sets to bind
			array of offfsets used for dynamic descriptors
		*/
		vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[CurrentFrame], 0, nullptr);
		// buffer, num of indices, instances, offset into index buffer, offset to add indices in index buffer, offset for instancing
		vkCmdDrawIndexed(CommandBuffer, static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(CommandBuffer);

		if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void DrawFrame()
	{
		/*
			Wait for the previous frame to finish
			Acquire an image from the swap chain
			Record a command buffer which draws the scene onto that image
			Submit the recorded command buffer
			Present the swap chain image
		*/

		// device, fence count, array of fences, wait for all fences complete?, timeout amount
		vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t ImageIndex;
		/*
		vkAcquireNextImageKHR
			VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a Window resize.
		vkQueuePresentKHR
			VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
		*/
		// logical device, swap chain for image, timeout in nanosec for img to become available(UINT64_MAX = basically disabled timeout), 
		// object to signal when Presentation engine is finished with image, null, output the index of avaialbe image in the swap chain 
		VkResult Result = vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &ImageIndex);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image");
		}


		UpdateUniformBuffer(CurrentFrame);

		// reset to unsignaled state if we want to submit work
		vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);

		// the buffer, VkCommandBufferResetFlagBits flag
		vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);

		RecordCommandBuffer(CommandBuffers[CurrentFrame], ImageIndex);


		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore WaitSemaphores[]{ ImageAvailableSemaphores[CurrentFrame] };
		VkPipelineStageFlags WaitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		// which semaphores and in which stage of the pipeline to wait on before execution begins
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = WaitSemaphores;
		SubmitInfo.pWaitDstStageMask = WaitStages;
		// which command buffer to submit for execution
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];

		// which semaphore to signal once execution finishes
		VkSemaphore SignalSemaphores[]{ RenderFinishedSemaphore[CurrentFrame] };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		// submit to graphics queue
		if (vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer");
		}

		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		// which semaphore to wait on before presentation
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = SignalSemaphores;

		VkSwapchainKHR SwapChains[]{ SwapChain };
		PresentInfo.swapchainCount = 1;
		// swap chain to present images to
		PresentInfo.pSwapchains = SwapChains;
		// index of image for each swap chain
		PresentInfo.pImageIndices = &ImageIndex;
		//optional, specify array of VkResult values to chekc for every individual swap chain if presentation successful
		PresentInfo.pResults = nullptr;

		// submit request to present image to swap chain
		Result = vkQueuePresentKHR(PresentQueue, &PresentInfo);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || FramebufferrResized)
		{
			FramebufferrResized = false;
			RecreateSwapChain();
		}
		else if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain image");
		}

		CurrentFrame = (CurrentFrame++) % MAX_FRAMES_IN_FLIGHT;
	}

	void UpdateUniformBuffer(uint32_t CurrentImage)
	{
		// constant rotation using delta Time per frame
		static auto StartTime = std::chrono::high_resolution_clock::now();

		auto CurrentTime = std::chrono::high_resolution_clock::now();

		float Time = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

		UniformBufferObject UBO{};
		// existing transformation, rotation angle, rotation axis
		// glm::mat4(1.0f) -> Identity matrix
		UBO.Model = glm::rotate(glm::mat4(1.0f), Time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// eye pos, center pos, up axis
		UBO.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// FOV, aspect ratio, near view plane, far view plane
		// use current swapchain extent to find aspect ratio in case of window resize
		UBO.Projection = glm::perspective(glm::radians(45.0f), SwapChainExtent.width / static_cast<float>(SwapChainExtent.height), 0.1f, 10.0f);

		// Y coord of the clip is inverted, need to flip
		UBO.Projection[1][1] *= -1;

		memcpy(UniformBuffersMapped[CurrentImage], &UBO, sizeof(UBO));
	}

	void CreateDescriptorPool()
	{
		// can't be created directly, nned to be allocated
		std::array<VkDescriptorPoolSize, 2> PoolSize{};
		PoolSize.at(0).type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		PoolSize.at(0).descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		PoolSize.at(1).type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		PoolSize.at(1).descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo PoolInfo{};
		PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		PoolInfo.poolSizeCount = static_cast<uint32_t>(PoolSize.size());
		PoolInfo.pPoolSizes = PoolSize.data();
		PoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(Device, &PoolInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool");
		}
	}

	void CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> Layouts(MAX_FRAMES_IN_FLIGHT, DescriptorSetLayout);

		VkDescriptorSetAllocateInfo AllocateInfo{};
		AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		AllocateInfo.descriptorPool = DescriptorPool;
		AllocateInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		AllocateInfo.pSetLayouts = Layouts.data();

		DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		if (vkAllocateDescriptorSets(Device, &AllocateInfo, DescriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		// populate every descriptor
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo BufferInfo{};
			BufferInfo.buffer = UniformBuffers[i];
			BufferInfo.offset = 0;
			// region containing data for the descriptor
			BufferInfo.range = sizeof(UniformBufferObject);

			/*
			If overwriting the whole buffer,
			then it is also possible to use the VK_WHOLE_SIZE value for the range
			*/

			VkDescriptorImageInfo ImageInfo{};
			ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ImageInfo.imageView = TextureImageView;
			ImageInfo.sampler = TextureSampler;

			// The configuration of descriptors is updated using the vkUpdateDescriptorSets function, takes an array of VkWriteDescriptorSet structs
			std::array<VkWriteDescriptorSet, 2> DescriptorWrite{};
			DescriptorWrite.at(0).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptor set to update and bind
			DescriptorWrite.at(0).dstSet = DescriptorSets[i];
			DescriptorWrite.at(0).dstBinding = 0;
			// descripotrs can be array, but not using array, so first index
			DescriptorWrite.at(0).dstArrayElement = 0;
			// the type of descriptor being updated, can update multiple at once
			DescriptorWrite.at(0).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			// how many elements to update
			DescriptorWrite.at(0).descriptorCount = 1;

			// for buffer data
			DescriptorWrite.at(0).pBufferInfo = &BufferInfo;
			// optionals
			// for image data
			DescriptorWrite.at(0).pImageInfo = nullptr;
			// for buffer view data
			DescriptorWrite.at(0).pTexelBufferView = nullptr;

			DescriptorWrite.at(1).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			DescriptorWrite.at(1).dstSet = DescriptorSets[i];
			DescriptorWrite.at(1).dstBinding = 1;
			DescriptorWrite.at(1).dstArrayElement = 0;
			DescriptorWrite.at(1).descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			DescriptorWrite.at(1).descriptorCount = 1;
			DescriptorWrite.at(1).pImageInfo = &ImageInfo;
			DescriptorWrite.at(1).pTexelBufferView = nullptr;

			/*
			It accepts two kinds of arrays :
				an array of VkWriteDescriptorSet
				an array of VkCopyDescriptorSet
			*/
			vkUpdateDescriptorSets(Device, static_cast<uint32_t>(DescriptorWrite.size()), DescriptorWrite.data(), 0, nullptr);
		}
	}


	void CreateSyncObjects()
	{
		ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
		InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo SemaphoreInfo{};
		SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo FenceInfo{};
		FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		// wait for fence returns immediately on the first pass
		FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &RenderFinishedSemaphore[i]) != VK_SUCCESS ||
				vkCreateFence(Device, &FenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame");
			}
		}

	}

	void CreateTextureImage()
	{
		int TexWidth, TexHeight, TexChannels;

		// array of pixels, 32 bits each
		stbi_uc* Pixels = stbi_load(TexturePath.c_str(), &TexWidth, &TexHeight, &TexChannels, STBI_rgb_alpha);
		VkDeviceSize ImageSize = TexWidth * TexHeight * 4;

		/* calculate Miplevels
		 * The max function selects the largest dimension.
		 * The log2 function calculates how many times that dimension can be divided by 2.
		 * The floor function handles cases where the largest dimension is not a power of 2.
		 * 1 is added so that the original image has a mip level.
		 */

		MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(TexWidth, TexHeight)))) + 1;

		if (!Pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}

		// copy pixels onto host visible memory to be used
		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;

		CreateBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			StagingBuffer, StagingBufferMemory);

		void* Data;
		vkMapMemory(Device, StagingBufferMemory, 0, ImageSize, 0, &Data);
		memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
		vkUnmapMemory(Device, StagingBufferMemory);

		// clean up pixel array
		stbi_image_free(Pixels);

		CreateImage(TexWidth, TexHeight,
			MipLevels,
			VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			// vkCmdBlitImage() is a transfer operation
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			TextureImage, TextureImageMemory);

		// copy staging buffer to texture image
		TransitionImageLayout(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, MipLevels);

		CopyBufferToImage(StagingBuffer, TextureImage, static_cast<uint32_t>(TexWidth), static_cast<uint32_t>(TexHeight));

		// transition to prepare for shader access
		// TransitionImageLayout(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, MipLevels);

		vkDestroyBuffer(Device, StagingBuffer, nullptr);
		vkFreeMemory(Device, StagingBufferMemory, nullptr);

		GenerateMipMaps(TextureImage, VK_FORMAT_R8G8B8A8_SRGB, TexWidth, TexHeight, MipLevels);
	}

	void GenerateMipMaps(VkImage Image, VkFormat ImageFormat,
		int32_t TextureWidth, int32_t TextureHeight,
		uint32_t MipLevels)
	{
		int32_t MipWidth = TextureWidth;
		int32_t MipHeight = TextureHeight;

		VkFormatProperties FormatProperties;
		vkGetPhysicalDeviceFormatProperties(PhysicalDevice, ImageFormat, &FormatProperties);
		if(!(FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("Texture image format does not support linear blitting");
		}

		VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier ImageBarrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.image = Image
		};

		ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageBarrier.subresourceRange.baseArrayLayer = 0;
		ImageBarrier.subresourceRange.layerCount = 1;
		ImageBarrier.subresourceRange.levelCount = 1;


		for(uint32_t i = 1; i < MipLevels; i++)
		{
			ImageBarrier.subresourceRange.baseMipLevel = i - 1;
			ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			ImageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			ImageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(CommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &ImageBarrier);

			VkImageBlit ImageBlit{};
			ImageBlit.srcOffsets[0] = { 0, 0, 0 };
			ImageBlit.srcOffsets[1] = { MipWidth, MipHeight, 1 };
			ImageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageBlit.srcSubresource.mipLevel = i - 1;
			ImageBlit.srcSubresource.baseArrayLayer = 0;
			ImageBlit.srcSubresource.layerCount = 1;
			ImageBlit.dstOffsets[0] = { 0, 0, 0 };
			ImageBlit.dstOffsets[1] = { MipWidth > 1 ? MipWidth / 2 : 1,
				MipHeight > 1 ? MipHeight / 2 : 1,
				// 2d image only have depth of 1
				1 };
			ImageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageBlit.dstSubresource.mipLevel = i;
			ImageBlit.dstSubresource.baseArrayLayer = 0;
			ImageBlit.dstSubresource.layerCount = 1;

			// must be submitted to queue with graphics capability
			vkCmdBlitImage(CommandBuffer,
				Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &ImageBlit,
				VK_FILTER_LINEAR);

			// for shader access
			ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			ImageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ImageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(CommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &ImageBarrier);

			// keep track of changes
			if (MipWidth > 1) MipWidth /= 2;
			if (MipHeight > 1) MipHeight /= 2;
		}

		// transition the very last mip level that wasn't handled by the loop
		ImageBarrier.subresourceRange.baseMipLevel = MipLevels - 1;
		ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		ImageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ImageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(CommandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr, 
			1, &ImageBarrier);

		EndSingleTimeCommands(CommandBuffer);
	}

	void CreateImage(uint32_t Width, uint32_t Height, 
		uint32_t MipLevels,
		VkSampleCountFlagBits NumSamples,
		VkFormat Format,
		VkImageTiling Tiling,
		VkImageUsageFlags Usage,
		VkMemoryPropertyFlags Properties,
		VkImage& Image, VkDeviceMemory& ImageMemory)
	{
		VkImageCreateInfo ImageInfo{};
		ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageInfo.extent.width = Width;
		ImageInfo.extent.height = Height;
		// the dimensions of the image, how many texels on each axis
		ImageInfo.extent.depth = 1;
		ImageInfo.mipLevels = MipLevels;
		ImageInfo.arrayLayers = 1;
		ImageInfo.format = Format;
		/*
		Tiling cannot be changed
		The tiling field can have one of two values:
			VK_IMAGE_TILING_LINEAR: Texels are laid out in row major order like our pixels array
			VK_IMAGE_TILING_OPTIMAL : Texels are laid out in an implementation defined order for optimal access
		*/

		ImageInfo.tiling = Tiling;
		ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ImageInfo.usage = Usage;
		// for multisampling
		ImageInfo.samples = NumSamples;
		// optional flags for sparce images, not using
		ImageInfo.flags = 0;
		ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(Device, &ImageInfo, nullptr, &Image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkMemoryRequirements MemRequirements;
		vkGetImageMemoryRequirements(Device, Image, &MemRequirements);

		VkMemoryAllocateInfo MemAllocInfo{};
		MemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		MemAllocInfo.allocationSize = MemRequirements.size;
		MemAllocInfo.memoryTypeIndex = FindMemoryType(MemRequirements.memoryTypeBits, Properties);

		if (vkAllocateMemory(Device, &MemAllocInfo, nullptr, &ImageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}

		vkBindImageMemory(Device, Image, ImageMemory, 0);
	}

	VkCommandBuffer BeginSingleTimeCommands()
	{
		// mem transfer are executed using cmd buffer
		VkCommandBufferAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandPool = CommandPool;
		AllocInfo.commandBufferCount = 1;

		VkCommandBuffer CommandBuffer;
		vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);


		// start recording command buffer
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

		return CommandBuffer;
	}

	void EndSingleTimeCommands(VkCommandBuffer& CommandBuffer)
	{
		// stop recording
		vkEndCommandBuffer(CommandBuffer);

		// execute cmd buffer
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer;

		vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
		// wait until the copy is idle (means completed), or use a fence to do the same thing
		vkQueueWaitIdle(GraphicsQueue);

		vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
	}

	// Image needs to be in right layout before drawing
	void TransitionImageLayout(VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, uint32_t MipLevels)
	{
		VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

		// use to ensure writes are compelete before reading
		// can also be used to transition image layouts and transfer queue family ownership when VK_SHARING_MODE_EXCLUSIVE is used
		VkImageMemoryBarrier MemBarrier{};
		MemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		MemBarrier.oldLayout = OldLayout;
		MemBarrier.newLayout = NewLayout;
		// transfer queue family ownership by giving its indicies, use IGNORED if not transfering
		MemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		MemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		MemBarrier.image = Image;
		MemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		MemBarrier.subresourceRange.baseMipLevel = 0;
		MemBarrier.subresourceRange.levelCount = MipLevels;
		MemBarrier.subresourceRange.baseArrayLayer = 0;
		MemBarrier.subresourceRange.layerCount = 1;

		/*
			MemBarrier.srcAccessMask = 0;
			MemBarrier.dstAccessMask = 0;
		*/

		VkPipelineStageFlags SourceStage;
		VkPipelineStageFlags DstStage;

		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			MemBarrier.srcAccessMask = 0;
			MemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			DstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			MemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			MemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			DstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition");
		}

		vkCmdPipelineBarrier(
			CommandBuffer,
			SourceStage, DstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &MemBarrier
		);

		EndSingleTimeCommands(CommandBuffer);
	}

	void CopyBufferToImage(VkBuffer Buffer, VkImage Image, uint32_t Width, uint32_t Height)
	{
		VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy Region{};
		// byte offset in buffer where pixels start
		Region.bufferOffset = 0;
		Region.bufferRowLength = 0;
		Region.bufferImageHeight = 0;

		Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Region.imageSubresource.mipLevel = 0;
		Region.imageSubresource.baseArrayLayer = 0;
		Region.imageSubresource.layerCount = 1;

		Region.imageOffset = { 0, 0, 0 };
		Region.imageExtent = { Width, Height, 1 };

		// enqueue copy operation
		vkCmdCopyBufferToImage(
			CommandBuffer,
			Buffer,
			Image,
			// image layout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&Region
		);

		EndSingleTimeCommands(CommandBuffer);
	}

};

int main()
{
	HelloTriangleApplication App{};

	try
	{
		App.Run();
	}
	catch (const std::exception& Error)
	{
		std::cerr << Error.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}