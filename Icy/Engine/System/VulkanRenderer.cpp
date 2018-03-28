#include "VulkanRenderer.hpp"
#include <iostream>
#include <set>

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
	if (!createSwapChain())
		return false;
	createImageViews();
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

	m_ValidationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

	// Create the instance
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensionStrings.data();
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
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
		if (result == vk::Result::eSuccess)
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
		vk::Bool32 presentSupport = m_PhysicalDevice.getSurfaceSupportKHR(i, m_Surface);
		if (prop.props.queueFlags & vk::QueueFlagBits::eGraphics)
			m_CurrentGraphicQueue = i;
		if (presentSupport)
			m_CurrentPresentQueue = i;
		m_QueueFamProps.push_back(prop);
	}
}

icy::SwapChainSupportDetails icy::System::VulkanRenderer::querySwapChainSupport()
{
	SwapChainSupportDetails scsd;
	scsd.capabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
	scsd.formats = m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface);
	scsd.presentModes = m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface);
	return scsd;
}

bool icy::System::VulkanRenderer::createLogicalDevice()
{
	// check for extensions
	auto extensions = m_PhysicalDevice.enumerateDeviceExtensionProperties();
	const char* swapchainName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	getQueueFamilies();
	std::vector<vk::DeviceQueueCreateInfo> dqci;
	std::set<int> uniqueQueueFamilies = { m_CurrentGraphicQueue, m_CurrentPresentQueue };
	float queuePrio = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo dqCreateInfo;
		dqCreateInfo.queueFamilyIndex = queueFamily;
		dqCreateInfo.queueCount = 1;
		dqCreateInfo.pQueuePriorities = &queuePrio;
		dqci.push_back(dqCreateInfo);
	}
	vk::PhysicalDeviceFeatures df = {};
	vk::DeviceCreateInfo dci = {};
	dci.queueCreateInfoCount = static_cast<uint32_t>(dqci.size());
	dci.pQueueCreateInfos = dqci.data();
	dci.pEnabledFeatures = &df;
	if (enableValidationLayers)
	{
		dci.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		dci.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		dci.enabledLayerCount = 0;
	}
	dci.enabledExtensionCount = 1;
	dci.ppEnabledExtensionNames = &swapchainName;
	auto result = m_PhysicalDevice.createDevice(&dci, nullptr, &m_Device);
	if (result == vk::Result::eSuccess)
	{
		m_GraphicsQueue = m_Device.getQueue(m_CurrentGraphicQueue, 0);
		m_PresentQueue = m_Device.getQueue(m_CurrentPresentQueue, 0);
		return true;
	}
	else
		return false;
}

bool icy::System::VulkanRenderer::createSwapChain()
{
	auto scs = querySwapChainSupport();
	auto surfaceFormat = chooseSwapSurfaceFormat(scs.formats);
	auto presentMode = chooseSwapPresentMode(scs.presentModes);
	m_SwapChainExtent = chooseSwapExtents(scs.capabilities);
	m_SwapChainFormat = surfaceFormat.format;

	auto imageCount = scs.capabilities.minImageCount + 1;
	if (scs.capabilities.maxImageCount > 0 && imageCount > scs.capabilities.maxImageCount)
	{
		imageCount = scs.capabilities.maxImageCount;
	}
	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = m_SwapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(m_CurrentGraphicQueue), static_cast<uint32_t>(m_CurrentPresentQueue)};
	if (m_CurrentGraphicQueue != m_CurrentPresentQueue)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = scs.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = vk::SwapchainKHR::SwapchainKHR();
	m_SwapChain = m_Device.createSwapchainKHR(createInfo, nullptr);
	if (m_SwapChain == vk::SwapchainKHR())
		return false;
	m_Image = m_Device.getSwapchainImagesKHR(m_SwapChain);
	return true;
}

vk::SurfaceFormatKHR icy::System::VulkanRenderer::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats)
{
	vk::SurfaceFormatKHR desiredFormat = formats[0];
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		desiredFormat.format = vk::Format::eB8G8R8A8Unorm;
		desiredFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
		return desiredFormat;
	}
	for (const auto& format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	return desiredFormat;
}

vk::PresentModeKHR icy::System::VulkanRenderer::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> modes)
{
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
	for (const auto& mode : modes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
		{
			return mode;
		}
		else if (mode == vk::PresentModeKHR::eImmediate)
		{
			bestMode = mode;
		}
	}
	return bestMode;
}

vk::Extent2D icy::System::VulkanRenderer::chooseSwapExtents(vk::SurfaceCapabilitiesKHR capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		vk::Extent2D actualExtent = { 800,500 };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void icy::System::VulkanRenderer::createImageViews()
{
	m_ImageView.resize(m_Image.size());
	for (int i = 0; i < m_Image.size(); i++)
	{
		vk::ImageViewCreateInfo createInfo;
		createInfo.image = m_Image[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = m_SwapChainFormat;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		m_ImageView[i] = m_Device.createImageView(createInfo);
	}
}

bool icy::System::VulkanRenderer::createGraphicsPipeline()
{
	return false;
}

void icy::System::VulkanRenderer::cleanup()
{
	if (enableValidationLayers)
		DestroyDebugReportCallbackEXT(m_Instance, m_CallBack, nullptr);
	m_Device.destroySwapchainKHR(m_SwapChain);
	m_Instance.destroySurfaceKHR(m_Surface);
	m_Device.destroy();
	m_Instance.destroy();
}