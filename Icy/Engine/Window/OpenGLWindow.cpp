#include "OpenGLWindow.hpp"
#include <SDL\SDL.h>
#include <glad\glad.h>

icy::Window::OpenGLWindow::OpenGLWindow()
{
	m_bClosed = false;
}

icy::Window::OpenGLWindow::~OpenGLWindow()
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(m_RenderContext);

	// Destroy our window
	SDL_DestroyWindow(m_Window);

	// Shutdown SDL 2
	SDL_Quit();
}

bool icy::Window::OpenGLWindow::createWindow(const std::string title, const int xPos, const int yPos, const int width, const int height, const Uint32 flags)
{
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return false;
	// Create the window
	m_Window = SDL_CreateWindow(title.c_str(), xPos, yPos, width, height, flags);
	// If the window failed to create, return false
	if (m_Window == nullptr)
		return false;

	// Create a openGL renderer
	m_RenderContext = SDL_GL_CreateContext(m_Window);

	// Setup our openGL settings
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Setup glad to load the openGL functions
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	return true;
}
