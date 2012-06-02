#include "NEngine.hpp"

int main(int argc, char* argv[])
{
	SetColor(Blue);
	std::cout << "ENGINE INFO: ";
	ClearColor();
	std::cout << "Welcome to " << PACKAGE_STRING << "! Report bugs to " << PACKAGE_BUGREPORT << "!\n";
	if (!GetGame()->Init(512,512,"Astrostruct",argc,argv))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize the game!\n";
		GetGame()->CleanUp();
		return 1;
	}
	NWindow* MyWindow = GetGame()->GetScene()->AddWindow();
	MyWindow->SetTexture("window");
	MyWindow->SetScale(256,256);
	MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
	NButton* QuitButton = GetGame()->GetScene()->AddButton();
	QuitButton->SetTexture("button");
	QuitButton->SetParent(MyWindow);
	QuitButton->SetScale(64,32);
	QuitButton->SetText(_t("Quit"));
	QuitButton->SetPos(0,-32);
	NButton* PlayButton = GetGame()->GetScene()->AddButton();
	PlayButton->SetTexture("button");
	PlayButton->SetParent(MyWindow);
	PlayButton->SetScale(64,32);
	PlayButton->SetText(_t("Play®"));
	PlayButton->SetPos(0,32);
	NMap* MyMap = GetGame()->GetScene()->AddMap("grimy");
	MyMap->Init(16,16,16);
	NSound* CoinSound = GetGame()->GetScene()->AddSound("coin");
	NCamera* Camera = GetGame()->GetScene()->AddCamera();
	Camera->SetPos(glm::vec3(0,0,800));
	GetGame()->GetRender()->SetCamera(Camera);
	NText* FPSText = GetGame()->GetScene()->AddText(_t("cousine"), _t("FPS: 0"));
	FPSText->SetPos(0,FPSText->GetSize()/2.f);
	FPSText->SetColor(0,0,0,1);
	glm::vec3 WantedPosition(0,0,800);
	while(GetGame()->Running())
	{
		Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedPosition)/4.f);
		GetGame()->GetInput()->Poll();
		if (PlayButton->OnRelease())
		{
		    CoinSound->Play();
		}
		if (GetGame()->GetInput()->GetKey('W'))
		{
			WantedPosition += glm::vec3(0,6,0);
		} else if (GetGame()->GetInput()->GetKey('S'))
		{
			WantedPosition -= glm::vec3(0,6,0);
		}
		if (GetGame()->GetInput()->GetKey('A'))
		{
			WantedPosition -= glm::vec3(6,0,0);
		} else if (GetGame()->GetInput()->GetKey('D'))
		{
			WantedPosition += glm::vec3(6,0,0);
		}
		if (GetGame()->GetInput()->KeyChanged('Q') && GetGame()->GetInput()->GetKey('Q'))
		{
			if (MyMap->GetLevel()>1)
			{
				WantedPosition -= glm::vec3(0,0,MyMap->GetTileSize());
			}
			MyMap->ViewLevel(MyMap->GetLevel()-1);
		} else if (GetGame()->GetInput()->KeyChanged('E') && GetGame()->GetInput()->GetKey('E'))
		{
			if (MyMap->GetLevel()<MyMap->GetDepth()-1)
			{
				WantedPosition += glm::vec3(0,0,MyMap->GetTileSize());
			}
			MyMap->ViewLevel(MyMap->GetLevel()+1);
		}
		MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
		std::stringstream NewText(std::stringstream::in | std::stringstream::out);
		NewText << "FPS: " << (1.f/GetGame()->GetRender()->GetFrameTime());
		FPSText->SetText(NewText.str());
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
		if (GetGame()->GetInput()->GetKey(GLFW_KEY_ESC) || QuitButton->OnRelease())
		{
			GetGame()->Close();
		}
	}
	GetGame()->CleanUp();
	return 0;
}
