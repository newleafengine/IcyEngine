#include "VulkanRenderer.hpp"
#include <iostream>
#include <set>
#include <vector>

icy::System::VulkanRenderer::VulkanRenderer()
{
}

icy::System::VulkanRenderer::~VulkanRenderer()
{
}

// returns true if our vkResult was SUCCESS
bool icy::System::VulkanRenderer::checkResults(VkResult result)
{
	if (result == VK_SUCCESS)
		return true;
	else return false;
}

bool icy::System::VulkanRenderer::initVulkan(SDL_SysWMinfo win)
{
	createInstance();
	return false;
}

bool icy::System::VulkanRenderer::createInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Icy Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Icy Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// get extensions
	uint32_t count;
	auto result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> properties(count);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
	// get layers
	result = vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> layerProps(count);
	result = vkEnumerateInstanceLayerProperties(&count, layerProps.data());
	// stores the strings to the ext/layers we want
	std::vector<const char*> extensionNames;
	std::vector<const char*> layerNames;

	for (const auto& ext : properties)
	{
		extensionNames.push_back(ext.extensionName);
	}
	for (const auto& layers : layerProps)
	{
		layerNames.push_back(layers.layerName);
	}
	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = layerNames.size();
	createInfo.ppEnabledLayerNames = layerNames.data();

	checkResults(vkCreateInstance(&createInfo, nullptr, &m_instance));
}