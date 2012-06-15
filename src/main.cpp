#include "NEngine.hpp"

int main(int argc, char** argv)
{
	SetColor(Blue);
	std::cout << "ENGINE INFO: ";
	ClearColor();
	std::cout << "Welcome to " << PACKAGE_STRING << "! Report bugs to " << PACKAGE_BUGREPORT << "!\n";
	if (!GetGame()->Init(512,512,"Astrostruct",argv[0]))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize the game!\n";
		GetGame()->CleanUp();
		return 1;
	}
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
