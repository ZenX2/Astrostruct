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
    //Reset if we hit GUI, the mouse might be released or not hitting the gui anymore so we need to refresh.
    HitGUI = false;
    NCamera* Camera = GetGame()->GetRender()->GetCamera();
    //Set up our projected mouse coords, used for projecting the mouse into 3d space quickly and effeciently.
    glfwGetMousePos(&MouseX,&MouseY);
    MouseY = -MouseY+GetGame()->GetWindowHeight();
    AWorld = glm::unProject(glm::vec3(MouseX,MouseY,0),Camera->GetPerspViewMatrix(),Camera->GetPerspMatrix(),glm::vec4(0,0,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight()));
    BWorld = glm::unProject(glm::vec3(MouseX,MouseY,1),Camera->GetPerspViewMatrix(),Camera->GetPerspMatrix(),glm::vec4(0,0,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight()));

    MouseX += GetGame()->GetRender()->GetCamera()->GetRealPos().x;
    MouseY += GetGame()->GetRender()->GetCamera()->GetRealPos().y;
    if (Focus) //Make sure we can't trap key focus permanently
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
    //If our focus is directed elsewhere, return false!
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

//This hocus pocus handles an array of booleans to check if a key has changed, it's a little broken in the fact that it MUST be called continuously in order to detect the change. (So things like if (KeyDown('i') && KeyChanged('i')) won't work because KeyChanged() won't be called whenever key 'i' is up.)
bool NInput::KeyChanged(int Key)
{
    //If our focus is directed elsewhere, return false!
    if (Focus)
    {
        return false;
    }
    //Make sure we don't try to grab keys out of our array size, if you see this it means we need to increase our array size! FIXME: switch it to use vectors.
    if (Key > KeyCount || Key < 0)
    {
        std::stringstream Message;
        Message << "Attempted to grab key: " << Key << ", it's out of range!";
        GetGame()->GetLog()->Send("INPUT",1,Message.str());
        return false;
    }
    //FIXME: I direct the lower 10 keys for mouse keys! This is bad as whatever 10 keys for the keyboard won't work!
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

//Clumbsily abstracts glfw's ability to grab joysticks, I can only test it with an xbox controller so this won't have good support unless some other programmer really wants it.
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
    //Linearly interpolates between AWorld and BWorld generated in NInput::Poll(), allowing you to grab a position in 3d space projected from the mouse.
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
