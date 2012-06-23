#include "NEngine.hpp"

NGame::NGame()
{
    Width = 0;
    Height = 0;
    Run = true;
    WindowChanged = true;
    Server = false;
}

NGame::~NGame()
{
    NTerminal::SetColor(Blue);
    std::cout << "ENGINE INFO: ";
    NTerminal::ClearColor();
    std::cout << "Shutting down...\n";
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
        SignalInterceptor = new NSignalInterceptor();
        if (!glfwInit())
        {
            NTerminal::SetColor(Red);
            std::cout << "ENGINE ERROR: ";
            NTerminal::ClearColor();
            std::cout << "GLFW failed to initialize!\n";
            return Fail;
        }
        FileSystem = new NFileSystem(argv[0]);
        Lua = new NLua();
        Config = new NConfig("config");
        Scene = new NScene();
        Map = Scene->AddMap(Config->GetString("MapSkin"));
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
    SignalInterceptor = new NSignalInterceptor();
    Width = i_Width;
    Height = i_Height;
    if (!glfwInit())
    {
        NTerminal::SetColor(Red);
        std::cout << "ENGINE ERROR: ";
        NTerminal::ClearColor();
        std::cout << "GLFW failed to initialize!\n";
        return Fail;
    }
    FileSystem = new NFileSystem(argv[0]);
    Lua = new NLua();
    Config = new NConfig("config");
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
        NTerminal::SetColor(Red);
        std::cout << "ENGINE ERROR: ";
        NTerminal::ClearColor();
        std::cout << "GLFW failed to open a window!\n";
        glfwTerminate();
        return Fail;
    }
    glfwSetWindowTitle(Title.c_str());
    if (glewInit() != GLEW_OK)
    {
        NTerminal::SetColor(Red);
        std::cout << "ENGINE ERROR: ";
        NTerminal::ClearColor();
        std::cout << "GLEW failed to initialize!\n";
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
    Map = Scene->AddMap(Config->GetString("MapSkin"));
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
        NTerminal::SetColor(Blue);
        std::cout << "ENGINE INFO: ";
        NTerminal::ClearColor();
        std::cout << "Game window was closed!\n" << std::flush;
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
