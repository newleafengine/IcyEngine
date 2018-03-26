#pragma once

// check for windows, else system is linux
#ifdef _WIN32
constexpr bool isWindows = true;
#define VK_USE_PLATFORM_WIN32_KHR
#else
constexpr bool isWindows = false;
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan\vulkan.hpp>
#include <SDL\SDL_syswm.h>

// if we are in debug mode then enable the validation layers
#ifdef _DEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

namespace icy
{
	// This struct stores the vk::QueueFamilyProperties as well as the index to that queue family
	struct QueueFamilyProps
	{
		int index;
		vk::QueueFamilyProperties props;
	};

	namespace System
	{
		class VulkanRenderer
		{
		public:
			VulkanRenderer();
			~VulkanRenderer();
			// startup vulkan
			bool initVulkan(const SDL_SysWMinfo& sysInfo);
		private:
			// create a vulkan instance 
			bool createInstance();
			bool createSurface(const SDL_SysWMinfo& sysInfo);
			// create a debug protocall
			bool setupDebugCallback();
			// pick a physical device that will be able to render graphics
			bool pickPhysicalDevice();
			// stores all queue families in the m_QueueFamProps vector
			void getQueueFamilies();
			// creates a logical device which the vulkan api can communicate through
			bool createLogicalDevice();
			// cleanup vulkan
			void cleanup();
		private:
			// vulkan concepts
			std::vector<const char*> m_ValidationLayers;			// contains the validation layer names;
			vk::Instance m_Instance;								// instance handle
			VkDebugReportCallbackEXT m_CallBack;					// debug handle
			vk::PhysicalDevice m_PhysicalDevice;					// physical device handle
			std::vector<icy::QueueFamilyProps> m_QueueFamProps;		// family queue properties for all family queues
			int	m_CurrentGraphicQueue;								// the current queue who's index we will use to send graphical commands to
			vk::Device m_Device;									// logical device
			vk::Queue m_GraphicsQueue;								// the queue to send graphical commands to

			// screen concepts
			vk::SurfaceKHR m_Surface;								// the surface is created from either a x11 or winapi window
		};
	}
}