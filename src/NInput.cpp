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
    GetGame()->GetInput()->SetKey(character,(action != GLFW_RELEASE));
    //This is used by string handling to pop the last character on the stack (if it exists).
    if (character == GLFW_KEY_BACKSPACE)
    {
        GetGame()->GetInput()->PushStringInput(character);
    }
}

void GLFWCALL NMouseKeyInput(int character, int action)
{
    GetGame()->GetInput()->SetMouseKey(character,(action != GLFW_RELEASE));
}

void NInput::SetKey(int Key, bool IsDown)
{
    if (Key >= (int)Keys.size())
    {
        Keys.resize(Key+1,false);
        KeyMem.resize(Key+1,false);
        KeyChanged.resize(Key+1,false);
    }
    Keys[Key] = IsDown;
}

void NInput::SetMouseKey(int Key, bool IsDown)
{
    if (Key >= (int)MouseKeys.size())
    {
        MouseKeys.resize(Key+1,false);
        MouseKeyMem.resize(Key+1,false);
        MouseKeyChanged.resize(Key+1,false);
    }
    MouseKeys[Key] = IsDown;
}

NInput::NInput()
{
    Focus = false;
    HitGUI = false;
    if (GetGame()->IsServer())
    {
        return;
    }
    glfwSetCharCallback(NStringInput);
    glfwSetKeyCallback(NKeyInput);
    glfwSetMouseButtonCallback(NMouseKeyInput);
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
    //Check if keys have changed
    for (unsigned int i=0;i<Keys.size();i++)
    {
        if (KeyMem[i] != Keys[i])
        {
            KeyMem[i] = Keys[i];
            KeyChanged[i] = true;
        } else {
            KeyChanged[i] = false;
        }
    }
    for (unsigned int i=0;i<MouseKeys.size();i++)
    {
        if (MouseKeyMem[i] != MouseKeys[i])
        {
            MouseKeyMem[i] = MouseKeys[i];
            MouseKeyChanged[i] = true;
        } else {
            MouseKeyChanged[i] = false;
        }
    }
}

bool NInput::GetKey(int Key)
{
    //If our focus is directed elsewhere, return false!
    if (Focus)
    {
        return false;
    }
    if (Key > (int)Keys.size())
    {
        Keys.resize(Key+1,false);
        KeyMem.resize(Key+1,false);
        KeyChanged.resize(Key+1,false);
        return glfwGetKey(Key);
    }
    return Keys[Key];
}
//This hocus pocus handles an array of booleans to check if a key has changed.
bool NInput::GetKeyChanged(int Key)
{
    //If our focus is directed elsewhere, return false!
    if (Focus)
    {
        return false;
    }
    //Make sure our vectors are large enough to hold all our keys
    if (Key > (int)Keys.size())
    {
        Keys.resize(Key+1,false);
        KeyMem.resize(Key+1,false);
        KeyChanged.resize(Key+1,false);
    }
    return KeyChanged[Key];
}

bool NInput::GetMouseKeyChanged(int Key)
{
    if (Key > (int)MouseKeys.size())
    {
        MouseKeys.resize(Key+1,false);
        MouseKeyMem.resize(Key+1,false);
        MouseKeyChanged.resize(Key+1,false);
    }
    return MouseKeyChanged[Key];
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
    if (Key >= (int)MouseKeys.size())
    {
        MouseKeys.resize(Key+1,false);
        MouseKeyMem.resize(Key+1,false);
        MouseKeyChanged.resize(Key+1,false);
        return glfwGetMouseButton(Key); //Temporarily use this while we wait for the next poll to fill it in.
    }
    return MouseKeys[Key];
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
