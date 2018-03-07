#include "VulkanRenderer.hpp"

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
	return true;
}

void icy::System::VulkanRenderer::clearnup()
{
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
	std::string excludedLayer = "VK_LAYER_LUNARG_vktrace";
	for (auto &layer : layers)
	{
		if (layer.layerName == excludedLayer)
		{
			continue;
		}
		else
			layerStrings.push_back(layer.layerName);
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