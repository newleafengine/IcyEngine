#include "VulkanRenderer.hpp"
#include <iostream>

// the debug call back that outputs the info to the console
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
{
	std::cerr << "validation layer: " << msg << std::endl;
	return VK_FALSE;
}

// creates the debug report
VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

// destroys the debug callback
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
	cleanup();
}

// called when creating a vulkan instance
bool icy::System::VulkanRenderer::initVulkan(const SDL_SysWMinfo & sysInfo)
{
	if(!createInstance())
		return false;
	if (enableValidationLayers)
	{
		if (!setupDebugCallback())
			return false;
	}
	if (!createSurface(sysInfo))
	{
		return false;
	}
	if (!pickPhysicalDevice())
		return false;
	if (!createLogicalDevice())
		return false;
	return true;
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

	// Extensions
	auto extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> extensionStrings;
	for (auto & ext : extensions)
	{
		extensionStrings.push_back(ext.extensionName);
	}

	// Layers
	auto layers = vk::enumerateInstanceLayerProperties();
	std::string validationLayer = "VK_LAYER_LUNARG_standard_validation";
	for (auto &layer : layers)
	{
		if (layer.layerName == validationLayer)
		{
			m_ValidationLayers.push_back(layer.layerName);
		}
	}

	// Create the instance
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensionStrings.data();
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = m_ValidationLayers.size();
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	auto result = vk::createInstance(&createInfo, nullptr, &m_Instance);
	if (result == vk::Result::eSuccess)
		return true;
	return false;
}

bool icy::System::VulkanRenderer::createSurface(const SDL_SysWMinfo & sysInfo)
{

	if (sysInfo.subsystem == SDL_SYSWM_WINDOWS) // windows
	{
#ifdef _WIN32
		vk::Win32SurfaceCreateInfoKHR createInfo = {};
		createInfo.hwnd = sysInfo.info.win.window;
		createInfo.hinstance = sysInfo.info.win.hinstance;

		auto result = m_Instance.createWin32SurfaceKHR(&createInfo, nullptr, &m_Surface);
		if (result == vk::Result::eSuccess)
			return true;
#endif // _WIN32
	}
	else if (sysInfo.subsystem == SDL_SYSWM_X11) // linux
	{
#ifndef _WIN32
		vk::XlibSurfaceCreateInfoKHR createInfo = {};
		createInfo.dpy = sysInfo.info.X11.display;
		createInfo.window = sysInfo.info.X11.window;

		auto result = m_Instance.createXlibSurfaceKHR(&createInfo, nullptr, &m_Surface);
			return true;
#endif // !_WIN32
	}
	return false;
}

bool icy::System::VulkanRenderer::setupDebugCallback()
{
	vk::DebugReportCallbackCreateInfoEXT createInfo = {};
						vk::DebugReportFlagBitsEXT::eWarning |
						vk::DebugReportFlagBitsEXT::ePerformanceWarning |
						vk::DebugReportFlagBitsEXT::eError |
						vk::DebugReportFlagBitsEXT::eDebug;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(m_Instance, reinterpret_cast<const VkDebugReportCallbackCreateInfoEXT*>(&createInfo), nullptr, &m_CallBack) != VK_SUCCESS) {
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
	else if (phyDevices.size() == 1)
	{
		// if the size is 1, pick this device
		m_PhysicalDevice = phyDevices[0];
	}
	else
	{	// TODO multiple devices, poll them to pick the best one
		return false;
	}
	return true;
}

void icy::System::VulkanRenderer::getQueueFamilies()
{
	auto queueProps = m_PhysicalDevice.getQueueFamilyProperties();
	for (int i = 0; i < queueProps.size(); i++)
	{
		icy::QueueFamilyProps prop;
		prop.index = i;
		prop.props = queueProps[i];
		if (prop.props.queueFlags & vk::QueueFlagBits::eGraphics)
			m_CurrentGraphicQueue = i;
		m_QueueFamProps.push_back(prop);
	}
}

bool icy::System::VulkanRenderer::createLogicalDevice()
{
	// TODO check for extensions

	// check for layer
	

	vk::DeviceQueueCreateInfo dqCreateInfo;
	dqCreateInfo.queueFamilyIndex = m_CurrentGraphicQueue;
	dqCreateInfo.queueCount = 1;
	float queueProp = 1.0f;
	dqCreateInfo.pQueuePriorities = &queueProp;
	vk::PhysicalDeviceFeatures df = {};
	vk::DeviceCreateInfo dci = {};
	dci.pQueueCreateInfos = &dqCreateInfo;
	dci.queueCreateInfoCount = 1;
	dci.pEnabledFeatures = &df;
	if (enableValidationLayers)
	{
		dci.enabledLayerCount = m_ValidationLayers.size();
		dci.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		dci.enabledLayerCount = 0;
	}
	auto result = m_PhysicalDevice.createDevice(&dci, nullptr, &m_Device);
	if (result == vk::Result::eSuccess)
	{
		m_GraphicsQueue = m_Device.getQueue(m_CurrentGraphicQueue, 0);
		return true;
	}
	else
		return false;
}

void icy::System::VulkanRenderer::cleanup()
{
	if (enableValidationLayers)
		DestroyDebugReportCallbackEXT(m_Instance, m_CallBack, nullptr);
	m_Instance.destroySurfaceKHR(m_Surface);
	m_Device.destroy();
	m_Instance.destroy();
}