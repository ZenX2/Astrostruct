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
		GetGame()->CleanUp();
		return 1;
	}
	while(GetGame()->Running())
	{
		GetGame()->GetInput()->Poll();
		if (GetGame()->GetInput()->GetMouseKey(0))
		{
			NText* InfoText = GetGame()->GetScene()->AddText("opensans", "Text objects!");
			InfoText->SetMode(1);
			InfoText->SetPos(GetGame()->GetInput()->GetMouse());
		}
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
		if (GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
		{
			GetGame()->Close();
		}
	}
	GetGame()->CleanUp();
	return 0;
}
