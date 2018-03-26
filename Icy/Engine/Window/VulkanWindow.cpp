#include "VulkanWindow.hpp"
#include <SDL\SDL.h>
#include <iostream>

icy::Window::VulkanWindow::VulkanWindow()
{
}

icy::Window::VulkanWindow::~VulkanWindow()
{
}

bool icy::Window::VulkanWindow::createWindow(const std::string title, const int xPos, const int yPos, const int width, const int height, const Uint32 flags)
{
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return false;
	// Create the window
	m_Window = SDL_CreateWindow(title.c_str(), xPos, yPos, width, height, flags);
	// If the window failed to create, return false
	if (m_Window == nullptr)
		return false;

	SDL_SysWMinfo systemInfo;
	SDL_VERSION(&systemInfo.version);
	SDL_GetWindowWMInfo(m_Window, &systemInfo);

	if (m_VRenderer.initVulkan(systemInfo))
		return true;
	else
		return false;
}
