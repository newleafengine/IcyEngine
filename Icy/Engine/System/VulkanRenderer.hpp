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
		private:
			vk::Instance m_Instance;
		};
	}
}