#include "NEngine.hpp"

int main(int argc, char** argv)
{
    if (!GetGame()->Init(512,512,"Astrostruct",argc,argv))
    {
        GetGame()->CleanUp();
        return 1;
    }
    GetGame()->GetLog()->Send("ENGINE",2,std::string("Welcome to ") + PACKAGE_STRING + "! Report bugs to " + PACKAGE_BUGREPORT + "!");
    while(GetGame()->Running())
    {
        GetGame()->GetStateMachine()->Tick();
        GetGame()->GetInput()->Poll();
        GetGame()->GetScene()->Tick();
        GetGame()->GetRender()->Draw();
        GetGame()->Poll();
    }
    GetGame()->CleanUp();
    return 0;
}
