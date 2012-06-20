#include "NEngine.hpp"

NInput::NInput()
{
	if (GetGame()->IsServer())
	{
		return;
	}
	for (unsigned int i=0;i<KeyCount;i++)
	{
		Keys[i] = false;
	}
}

void NInput::Poll()
{
	if (GetGame()->IsServer())
	{
		return;
	}
	HitGUI = false;
	NCamera* Camera = GetGame()->GetRender()->GetCamera();
	glfwGetMousePos(&MouseX,&MouseY);
	MouseY = -MouseY+GetGame()->GetWindowHeight();
	AWorld = glm::unProject(glm::vec3(MouseX,MouseY,0),Camera->GetPerspViewMatrix(),Camera->GetPerspMatrix(),glm::vec4(0,0,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight()));
	BWorld = glm::unProject(glm::vec3(MouseX,MouseY,1),Camera->GetPerspViewMatrix(),Camera->GetPerspMatrix(),glm::vec4(0,0,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight()));

	MouseX += GetGame()->GetRender()->GetCamera()->GetPos().x;
	MouseY += GetGame()->GetRender()->GetCamera()->GetPos().y;
}

int NInput::GetKey(int Key)
{
	return glfwGetKey(Key);
}

bool NInput::KeyChanged(int Key)
{
	if (Key > KeyCount || Key < 0)
	{
		SetColor(Yellow);
		std::cout << "INPUT WARN: ";
		ClearColor();
		std::cout << "Attempted to grab key: " << Key << ", it's out of range!\n";
		return false;
	}
	if (Keys[Key] != GetKey(Key))
	{
		Keys[Key] = GetKey(Key);
		return true;
	}
	return false;
}

void NInput::GetJoyStick(unsigned int ID, float* Floats, unsigned int NumAxes)
{
	switch (ID)
	{
		case 1:
		{
			glfwGetJoystickPos(GLFW_JOYSTICK_1,Floats,NumAxes);
			break;
		}
		case 2:
		{
			glfwGetJoystickPos(GLFW_JOYSTICK_2,Floats,NumAxes);
			break;
		}
		default:
		{
			glfwGetJoystickPos(GLFW_JOYSTICK_1,Floats,NumAxes);
		}
	}
}

int NInput::GetMouseX()
{
	return MouseX;
}

int NInput::GetMouseY()
{
	return MouseY;
}

glm::vec3 NInput::GetPerspMouse(float Inter)
{
	return AWorld+Inter*(BWorld-AWorld);
}

glm::vec2 NInput::GetMouse()
{
	return glm::vec2(MouseX,MouseY);
}

int NInput::GetMouseKey(int Key)
{
	return glfwGetMouseButton(Key);
}
bool NInput::GetMouseHitGUI()
{
	return HitGUI;
}
void NInput::SetMouseHitGUI(bool Hit)
{
	HitGUI = Hit;
}
