#include "NEngine.hpp"

void GLFWCALL NStringInput(int character, int action)
{
    if (action == GLFW_RELEASE)
    {
        return;
    }
    GetGame()->GetInput()->PushStringInput(character);
}
void GLFWCALL NKeyInput(int character, int action)
{
    if (action == GLFW_RELEASE)
    {
        return;
    }
    if (character == GLFW_KEY_BACKSPACE)
    {
        GetGame()->GetInput()->PushStringInput(character);
    }
}

NInput::NInput()
{
    Focus = false;
    HitGUI = false;
    if (GetGame()->IsServer())
    {
        return;
    }
    for (unsigned int i=0;i<KeyCount;i++)
    {
        Keys[i] = false;
    }
    glfwSetCharCallback(NStringInput);
    glfwSetKeyCallback(NKeyInput);
    glfwEnable(GLFW_KEY_REPEAT);
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

    MouseX += GetGame()->GetRender()->GetCamera()->GetRealPos().x;
    MouseY += GetGame()->GetRender()->GetCamera()->GetRealPos().y;
    if (Focus) //Make sure we can't trap focus permanently
    {
        if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS || glfwGetKey(GLFW_KEY_ENTER))
        {
            Focus = false;
        }
    } else {
        StringInput.clear();
    }
}

bool NInput::GetKey(int Key)
{
    if (Focus)
    {
        return false;
    }
    if (glfwGetKey(Key) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}

bool NInput::KeyChanged(int Key)
{
    if (Focus)
    {
        return false;
    }
    if (Key > KeyCount || Key < 0)
    {
        NTerminal::SetColor(Yellow);
        std::cout << "INPUT WARN: ";
        NTerminal::ClearColor();
        std::cout << "Attempted to grab key: " << Key << ", it's out of range!\n";
        return false;
    }
    if (Key<10)
    {
        if (Keys[Key] != GetMouseKey(Key))
        {
            Keys[Key] = GetMouseKey(Key);
            return true;
        }
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
void NInput::SetFocus(bool i_Focus)
{
    Focus = i_Focus;
}
bool NInput::GetFocus()
{
    return Focus;
}
void NInput::PushStringInput(int String)
{
    StringInput.push_back(String);
}
std::vector<int> NInput::GetStringInput()
{
    std::vector<int> String = StringInput;
    StringInput.clear();
    return String;
}
