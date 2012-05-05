#include "NEngine.hpp"

void NInput::Poll()
{
	glfwGetMousePos(&MouseX,&MouseY);
	MouseY = -MouseY+GetGame()->GetWindowHeight();
}

int NInput::GetKey(int Key)
{
	return glfwGetKey(Key);
}

bool NInput::KeyChanged(int Key)
{
	if (Key > 128 || Key < 0)
	{
		SetColor(Yellow);
		std::cout << "INPUT WARN: ";
		ClearColor();
		std::cout << "Attempted to grab a key out of range!\n";
		return false;
	}
	if (Keys[Key] != GetKey(Key))
	{
		Keys[Key] = GetKey(Key);
		return true;
	}
	return false;
}

int NInput::GetMouseX()
{
	return MouseX;
}

int NInput::GetMouseY()
{
	return MouseY;
}

glm::vec2 NInput::GetMouse()
{
	return glm::vec2(MouseX,MouseY);
}

int NInput::GetMouseKey(int Key)
{
	return glfwGetMouseButton(Key);
}
