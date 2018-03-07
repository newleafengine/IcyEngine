#include <Engine\Window\VulkanWindow.hpp>
#include <glad\glad.h>

int main()
{
	icy::Window::VulkanWindow window;
	window.createWindow("Hello App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
	while (window.isOpen())
		window.display();
	return 1;
}