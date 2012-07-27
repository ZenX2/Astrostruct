#include "NEngine.hpp"

NGame::NGame()
{
    NewWidth = 0;
    NewHeight = 0;
    Width = 0;
    Height = 0;
    Run = true;
    WindowChanged = true;
    Server = false;
    StateMachine = NULL;
    PacketHandler = NULL;
    LightSystem = NULL;
    FileSystem = NULL;
    Input = NULL;
    Scene = NULL;
    Render = NULL;
    Lua = NULL;
    Config = NULL;
    SoundSystem = NULL;
    TextSystem = NULL;
    SignalInterceptor = NULL;
    Console = NULL;
    Map = NULL;
    Network = NULL;
    Log = NULL;
}

NGame::~NGame()
{
    Log->Send("ENGINE",2,"Shutting down...");
    delete SignalInterceptor;
    delete FileSystem;
    delete Scene;
    delete Console;
    delete Config;
    delete Lua;
    delete Network;
    delete StateMachine;
    delete Input;
    delete Render;
    delete PacketHandler;
    if (!Server)
    {
        delete SoundSystem;
        delete TextSystem;
        delete LightSystem;
    }
    delete Log;
    glfwTerminate();
}

bool NGame::Init(int i_Width, int i_Height, std::string Title, int argc, char** argv)
{
    for (unsigned int i=0;i<argc;i++)
    {
        std::string Option = argv[i];
        if (Option == "-s")
        {
            Server = true;
        }
    }
    if (Server)
    {
        srand(time(NULL));
        FileSystem = new NFileSystem(argv[0]);
        Log = new NLog();
        if (!glfwInit())
        {
            Log->Send("ENGINE",0,"GLFW failed to initialize!");
            return Fail;
        }
        Lua = new NLua();
        Config = new NConfig("config");
        SignalInterceptor = new NSignalInterceptor();
        Scene = new NScene();
        Map = new NMap(Config->GetString("MapSkin"));
        StateMachine = new NStateMachine("Serving");
        Network = new NNetwork();
        Network->CreateServer();
        Input = new NInput();
        Render = new NRender();
        Console = new NConsole();
        PacketHandler = new NPacketHandler();
        return Success;
    }
    //Initialize everything we can
    srand(time(NULL));
    FileSystem = new NFileSystem(argv[0]);
    Log = new NLog();
    Lua = new NLua();
    Config = new NConfig("config");
    SignalInterceptor = new NSignalInterceptor();
    Width = i_Width;
    Height = i_Height;
    if (!glfwInit())
    {
        Log->Send("ENGINE",0,"GLFW failed to initialize!");
        return Fail;
    }
    //Now lets load some data from our config interface
    NewWidth = Config->GetFloat("Width");
    NewHeight = Config->GetFloat("Height");
    if (NewWidth != 0)
    {
        Width = NewWidth;
    }
    if (NewHeight != 0)
    {
        Height = NewHeight;
    }
    int Check = 0;
    if (Config->GetBool("Fullscreen"))
    {
        Check = glfwOpenWindow(Width,Height,5,6,5,0,8,8,GLFW_FULLSCREEN);
    } else {
        Check = glfwOpenWindow(Width,Height,5,6,5,0,8,8,GLFW_WINDOW);
    }
    if (!Check)
    {
        Log->Send("ENGINE",0,"GLFW failed to open a window!");
        glfwTerminate();
        return Fail;
    }
    glfwSetWindowTitle(Title.c_str());
    if (glewInit() != GLEW_OK)
    {
        Log->Send("ENGINE",0,"GLEW failed to initialize!");
        glfwTerminate();
        return Fail;
    }
    glfwSetWindowSizeCallback(&ResizeWindow);
    Input = new NInput();
    Scene = new NScene();
    Render = new NRender();
    Render->LoadTextures();
    SoundSystem = new NSoundSystem();
    SoundSystem->LoadSounds();
    TextSystem = new NTextSystem();
    TextSystem->LoadFaces();
    Map = new NMap(Config->GetString("MapSkin"));
    Console = new NConsole();
    LightSystem = new NLightSystem();
    StateMachine = new NStateMachine("Paused");
    Network = new NNetwork();
    PacketHandler = new NPacketHandler();
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

glm::vec2 NGame::GetWindowSize()
{
    return glm::vec2(Width,Height);
}

void NGame::Poll()
{
    if (Server)
    {
        return;
    }
    //If the window closes, end the game!
    if (!glfwGetWindowParam(GLFW_OPENED))
    {
        Log->Send("ENGINE",2,"Game window was closed!");
        Close();
        return;
    }
    //If the window has changed, reset our opengl view matrix and context size.
    if (Width != NewWidth || Height != NewHeight)
    {
        Width = NewWidth;
        Height = NewHeight;
        WindowChanged = true;
        Render->SetSize(Width,Height);
    } else {
        WindowChanged = false;
    }
}

void ResizeWindow(int W, int H)
{
    GetGame()->NewWidth = W;
    GetGame()->NewHeight = H;
}

void NGame::SetWindowChanged(bool Changed)
{
    WindowChanged = Changed;
}

bool NGame::GetWindowChanged()
{
    return WindowChanged;
}

NLua* NGame::GetLua()
{
    return Lua;
}

NConfig* NGame::GetConfig()
{
    return Config;
}

void NGame::SetWindowSize(int W, int H)
{
    glfwSetWindowSize(W,H);
    NewWidth = W;
    NewHeight = H;
}

void NGame::CleanUp()
{
    delete this;
}

NSoundSystem* NGame::GetSoundSystem()
{
    return SoundSystem;
}

NTextSystem* NGame::GetTextSystem()
{
    return TextSystem;
}

NFileSystem* NGame::GetFileSystem()
{
    return FileSystem;
}

NMap* NGame::GetMap()
{
    return Map;
}

NLightSystem* NGame::GetLightSystem()
{
    return LightSystem;
}

NStateMachine* NGame::GetStateMachine()
{
    return StateMachine;
}

bool NGame::IsServer()
{
    return Server;
}

NNetwork* NGame::GetNetwork()
{
    return Network;
}
NPacketHandler* NGame::GetPacketHandler()
{
    return PacketHandler;
}
NLog* NGame::GetLog()
{
    return Log;
}
