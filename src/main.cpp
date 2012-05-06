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
	NText* FPSText = GetGame()->GetScene()->AddText("opensans", "FPS: 0");
	FPSText->SetPos(0,FPSText->GetSize()/2.f);
	FPSText->SetColor(0,0,0,1);
	while(GetGame()->Running())
	{
		GetGame()->GetInput()->Poll();
		if (GetGame()->GetInput()->GetMouseKey(0))
		{
			std::string Text;
			for (unsigned int i=0;i<30;i++)
			{
				char MyChar = 32+rand()%128;
				Text += MyChar;
			}
			NText* YayText = GetGame()->GetScene()->AddText("opensans",Text);
			YayText->SetPos(GetGame()->GetInput()->GetMouse());
			YayText->SetAng(Rand(0,360));
			YayText->SetSize(Rand(8,72));
			YayText->SetColor(Rand(0,1),Rand(0,1),Rand(0,1),Rand(0,1));
		}
		std::stringstream NewText(std::stringstream::in | std::stringstream::out);
		NewText << "FPS: " << 1/GetGame()->GetRender()->GetFrameTime() << '\0';
		std::string FinalText = NewText.str();
		FPSText->SetText(FinalText);
		FPSText->SwapDepth(GetGame()->GetScene()->GetTopDepth());
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
