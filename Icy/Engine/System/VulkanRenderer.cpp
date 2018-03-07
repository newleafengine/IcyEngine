#include "VulkanRenderer.hpp"
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) 
{
	std::cerr << "validation layer: " << msg << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}



icy::System::VulkanRenderer::VulkanRenderer()
{
}

icy::System::VulkanRenderer::~VulkanRenderer()
{
}

bool icy::System::VulkanRenderer::initVulkan()
{
	auto result = createInstance();
	if(!result)
		return false;
	result = setupDebugCallback();
	if (!result)
		return false;
	result = pickPhysicalDevice();
	return true;
}

void icy::System::VulkanRenderer::clearnup()
{
	DestroyDebugReportCallbackEXT(m_Instance, m_CallBack, nullptr);
	m_Instance.destroy();
}

bool icy::System::VulkanRenderer::createInstance()
{
	// Application info
	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Icy Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Layer and extensions
	auto extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> extensionStrings;
	for (auto & ext : extensions)
	{
		extensionStrings.push_back(ext.extensionName);
	}

	auto layers = vk::enumerateInstanceLayerProperties();
	std::vector<const char*> layerStrings;
	std::string validationLayer = "VK_LAYER_LUNARG_standard_validation";
	for (auto &layer : layers)
	{
		if (layer.layerName == validationLayer)
		{
			layerStrings.push_back(layer.layerName);
		}
	}

	// Create the instance
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensionStrings.data();
	createInfo.enabledLayerCount = layerStrings.size();
	createInfo.ppEnabledLayerNames = layerStrings.data();

	auto result = vk::createInstance(&createInfo, nullptr, &m_Instance);

	if (result == vk::Result::eSuccess)
		return true;
	return false;
}

bool icy::System::VulkanRenderer::setupDebugCallback()
{
	VkDebugReportCallbackCreateInfoEXT createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(m_Instance, &createInfo, nullptr, &m_CallBack) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
		return false;
	}
	return true;
}

bool icy::System::VulkanRenderer::pickPhysicalDevice()
{
	auto phyDevices = m_Instance.enumeratePhysicalDevices();
	if (phyDevices.size() == 0)
	{
		std::cout << "Device count == 0, no GPU found\n";
	}
	bool deviceFound = false;
	int score = 0;
	for (const auto& device : phyDevices)
	{
		m_PhysicalDevice = device;
		if (isDeviceSuitable())
		{
			deviceFound = true;
			int currentScore = getDeviceScore();
			if (score < currentScore)
				score = currentScore;
		}
	}

	if (!deviceFound)
		return false;

}

bool icy::System::VulkanRenderer::isDeviceSuitable()
{
	auto properties = m_PhysicalDevice.getProperties();
	auto features = m_PhysicalDevice.getFeatures();
	return true;
}

int icy::System::VulkanRenderer::getDeviceScore()
{
	return 0;
}
