#pragma once
#include <vulkan\vulkan.hpp>

namespace icy
{
	namespace System
	{
		class VulkanRenderer
		{
		public:
			VulkanRenderer();
			~VulkanRenderer();
			bool initVulkan();
			void clearnup();
		private:
			bool createInstance();
			bool setupDebugCallback();
			bool pickPhysicalDevice();
		private:
			vk::Instance m_Instance;
			VkDebugReportCallbackEXT m_CallBack;
			vk::PhysicalDevice m_PhysicalDevice;
		};
	}
}