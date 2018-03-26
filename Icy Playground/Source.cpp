#include <Engine\Window\VulkanWindow.hpp>
#include <glad\glad.h>

int main()
{
	icy::Window::VulkanWindow window;
	if (window.createWindow("Hello Triangle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN))
	{
		while (window.isOpen())
		{
			SDL_Event ev;
			while (window.pollEvents(&ev))
			{
				if (ev.type == SDL_KEYDOWN)
				{
					if (ev.key.keysym.sym == SDLK_ESCAPE)
					{
						window.close();
					}
				}
			}
		}
		window.close();
	}
}