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
