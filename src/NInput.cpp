#include "NEngine.hpp"

void NInput::Poll()
{
	glfwGetMousePos(&MouseX,&MouseY);
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
