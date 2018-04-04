#pragma once
// check for windows, else system is linux
#ifdef _WIN32
constexpr bool isWindows = true;
#define VK_USE_PLATFORM_WIN32_KHR
#else
constexpr bool isWindows = false;
#define VK_USE_PLATFORM_XLIB_KHR
#endif
// if we are in debug mode then enable the validation layers
#ifdef _DEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

#include <vulkan\vulkan.h>
#include <SDL\SDL_syswm.h>
// undef these since they are included by SDL
#undef max
#undef min

namespace icy
{
	namespace System
	{
		class VulkanRenderer
		{
		public:
			VulkanRenderer();
			~VulkanRenderer();
			bool checkResults(VkResult results);
			bool initVulkan(SDL_SysWMinfo win);
			bool createInstance();
		private:

		private:
			VkInstance m_instance;
		};
	}
}