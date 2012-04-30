#include "NEngine.hpp"

int main(int argc, char** argv)
{
	SetColor(Blue);
	std::cout << "ENGINE INFO: ";
	ClearColor();
	std::cout << "Weclome to " << PACKAGE_STRING << "! Report bugs to " << PACKAGE_BUGREPORT << "!\n";
	if (!GetGame()->Init(512,512,"Astrostruct"))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize the game!\n";
		return 1;
	}
	//Initialize game stuff here
	Text* MyText = GetGame()->GetTextSystem()->AddText("opensans", "Dang");
	MyText->SetMode(1);
	MyText->SetPos(GetGame()->GetWindowSize()/2.f);
	Text* MyText2 = GetGame()->GetTextSystem()->AddText("opensans", "owned");
	MyText2->SetPos(50,0);
	MyText2->SetParent(MyText);
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
