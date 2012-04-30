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
		delete TextSystem;
		glfwTerminate();
	}
}

bool NGame::Init(int i_Width, int i_Height, std::string Title)
{
	Width = i_Width;
	Height = i_Height;
	if (!glfwInit())
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLFW failed to initialize!\n";
		return Fail;
	}
	if (!glfwOpenWindow(Width,Height,0,0,0,0,0,0,GLFW_WINDOW))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLFW failed to open a window!\n";
		glfwTerminate();
		return Fail;
	}
	glfwSetWindowTitle(Title.c_str());
	if (glewInit() != GLEW_OK)
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "GLEW failed to initialize!\n";
		glfwTerminate();
		return Fail;
	}
	glfwSetWindowSizeCallback(&ResizeWindow);
	Input = new NInput();
	Scene = new NScene();
	Render = new NRender();
	TextSystem = new NTextSystem();
	TextSystem->LoadFaces();
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

NTextSystem* NGame::GetTextSystem()
{
	return TextSystem;
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

glm::vec2 NGame::GetWindowSize()
{
	return glm::vec2(Width,Height);
}

void NGame::Poll()
{
	if (!glfwGetWindowParam(GLFW_OPENED))
	{
		SetColor(Blue);
		std::cout << "ENGINE INFO: ";
		ClearColor();
		std::cout << "Game window was closed, exiting...\n" << std::flush;
		Close();
		return;
	}
	if (Width != NewWidth || Height != NewHeight)
	{
		Width = NewWidth;
		Height = NewHeight;
		glViewport(0,0,Width,Height);
		WindowChanged = true;
	} else {
		WindowChanged = false;
	}
}

void ResizeWindow(int X, int Y)
{
	GetGame()->NewWidth = X;
	GetGame()->NewHeight = Y;
}

void NGame::SetWindowChanged(bool Changed)
{
	WindowChanged = Changed;
}

bool NGame::GetWindowChanged()
{
	return WindowChanged;
}
