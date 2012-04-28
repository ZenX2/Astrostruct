#include "NEngine.hpp"

NGame::NGame()
{
	Width = 0;
	Height = 0;
	Run = true;
	Valid = false;
}

NGame::~NGame()
{
	if (Valid)
	{
		delete Input;
		delete Scene;
		delete Render;
		glfwTerminate();
	}
}

bool NGame::Init(int i_Width, int i_Height, std::string Title)
{
	Width = i_Width;
	Height = i_Height;
	if (!glfwInit())
	{
		std::cout << "ENGINE ERROR: GLFW failed to initialize!\n";
		return Fail;
	}
	if (!glfwOpenWindow(Width,Height,0,0,0,0,0,0,GLFW_WINDOW))
	{
		std::cout << "ENGINE ERROR: GLFW failed to open a window!\n";
		glfwTerminate();
		return Fail;
	}
	glfwSetWindowTitle(Title.c_str());
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ENGINE ERROR: GLEW failed to initialize!\n";
		glfwTerminate();
		return Fail;
	}
	Input = new NInput();
	Scene = new NScene();
	Render = new NRender();
	Valid = true;
	return Success;
}

NInput* NGame::GetInput()
{
	return Input;
}

NScene* NGame::GetScene()
{
	return Scene;
}

NRender* NGame::GetRender()
{
	return Render;
}

bool NGame::Running()
{
	return Run;
}

void NGame::Close()
{
	Run = false;
}

int NGame::GetWindowWidth()
{
	return Width;
}

int NGame::GetWindowHeight()
{
	return Height;
}

void NGame::Poll()
{
	if (!glfwGetWindowParam(GLFW_OPENED))
	{
		std::cout << "ENGINE INFO: Game window was closed, exiting...\n" << std::flush;
		Close();
		return;
	}
}
