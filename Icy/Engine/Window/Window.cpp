#include "Window.hpp"
#include <SDL\SDL.h>
#include <glad\glad.h>

icy::Window::Window::Window()
{
	m_bClosed = false;
}