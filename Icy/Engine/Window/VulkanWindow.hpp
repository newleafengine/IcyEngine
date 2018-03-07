#pragma once
#include "Window.hpp"
#include <Engine\System\VulkanRenderer.hpp>

namespace icy
{
	namespace Window
	{
		class VulkanWindow : public Window
		{
		public:
			VulkanWindow();
			virtual ~VulkanWindow();
			// Creates a window
			// title : The title of the Window
			// xPos : The left position of the window
			// yPos : The top position of the window
			// width : The width of the Window
			// height : The height of the Window
			// flags : SDL flags, look at SDL wiki for more info
			virtual bool createWindow(const std::string title, const int xPos, const int yPos, const int width, const int height, const Uint32 flags);
			// Checks if the window is still valid and opened
			virtual inline bool isOpen() { return !m_bClosed; }
			virtual inline void close() { m_bClosed = true; }
			virtual inline void display() { SDL_GL_SwapWindow(m_Window); }

		private:
			icy::System::VulkanRenderer m_VRenderer;
		};
	}
}