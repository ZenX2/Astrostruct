#include "NEngine.hpp"

int main(int argc, char** argv)
{
	if (!GetGame()->Init(512,512,"Astrostruct"))
	{
		std::cout << "GAME ERROR: Failed to initialize the game!\n";
		return 1;
	}
	while(GetGame()->Running())
	{
		GetGame()->GetInput()->Poll();
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
	}
	CleanUp();
	return 0;
}
