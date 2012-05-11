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
	NWindow* MyWindow = GetGame()->GetScene()->AddWindow();
	MyWindow->SetTexture("runner");
	MyWindow->SetScale(256,256);
	NText* FPSText = GetGame()->GetScene()->AddText("cousine", "FPS: 0");
	FPSText->SetPos(0,FPSText->GetSize()/2.f);
	FPSText->SetColor(0,0,0,1);
	NText* CountText = GetGame()->GetScene()->AddText("didactgothic", "Glyphs Rendered: 0");
	CountText->SetPos(0,FPSText->GetPos().y+CountText->GetSize()/2.f+10);
	CountText->SetColor(0,0,0,1);
	unsigned int TextCount = 30;
	while(GetGame()->Running())
	{
		GetGame()->GetInput()->Poll();
		MyWindow->SetPos(GetGame()->GetInput()->GetMouse());
		if (GetGame()->GetInput()->GetMouseKey(0))
		{
			std::string Text;
			for (unsigned int i=0;i<30;i++)
			{
				char MyChar = 32+rand()%128;
				if (!isprint(MyChar))
				{
					i--;
					continue;
				}
				Text += MyChar;
			}
			NText* YayText = GetGame()->GetScene()->AddText("gtw",Text);
			YayText->SetPos(GetGame()->GetInput()->GetMouse());
			YayText->SetAng(Rand(0,360));
			YayText->SetSize(Rand(8,72));
			YayText->SetColor(Rand(0,1),Rand(0,1),Rand(0,1),Rand(0,1));
			TextCount += 30;
		}
		if (GetGame()->GetInput()->GetMouseKey(1))
		{
			NWindow* NewWindow = GetGame()->GetScene()->AddWindow();
			NewWindow->SetTexture("runner");
			NewWindow->SetScale(256,256);
			NewWindow->SetPos(GetGame()->GetInput()->GetMouse());
		}
		std::stringstream NewText(std::stringstream::in | std::stringstream::out);
		NewText << "FPS: " << 1/GetGame()->GetRender()->GetFrameTime() << '\0';
		FPSText->SetText(NewText.str());
		FPSText->SwapDepth(GetGame()->GetScene()->GetTopDepth());
		std::stringstream NewText2(std::stringstream::in | std::stringstream::out);
		NewText2 << "Glyphs Rendered: " << TextCount << '\0';
		CountText->SetText(NewText2.str());
		CountText->SwapDepth(GetGame()->GetScene()->GetTopDepth());
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
