#pragma once
#include <string>
#include <SDL\SDL_render.h>
#include <SDL\SDL_events.h>
namespace icy
{
	namespace Window 
	{
		class Window
		{
		public:
			Window();
			virtual ~Window() {};
			// Creates a window
			// title : The title of the Window
			// xPos : The left position of the window
			// yPos : The top position of the window
			// width : The width of the Window
			// height : The height of the Window
			// flags : SDL flags, look at SDL wiki for more info
			virtual bool createWindow(const std::string title, const int xPos, const int yPos, const int width, const int height, const Uint32 flags) = 0;
			virtual bool pollEvents(SDL_Event* ev) { return SDL_PollEvent(ev); }
			// Checks if the window is still valid and opened
			virtual bool isOpen() = 0;
			virtual void close() = 0;
			virtual void display() = 0;
			SDL_Window * m_Window;
			SDL_GLContext m_RenderContext;
			bool m_bClosed;
		};
	}
}