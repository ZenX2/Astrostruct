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
	NSound* CoinSound = GetGame()->GetScene()->AddSound("coin");
	NCamera* Camera = GetGame()->GetScene()->AddCamera();
	Camera->SetPos(glm::vec3(0,0,500));
	GetGame()->GetRender()->SetCamera(Camera);
	glm::vec3 WantedPosition(0,0,500);
	NText* FPSText = GetGame()->GetScene()->AddText("cousine", _t("FPS: 0"));
	FPSText->SetPos(0,FPSText->GetSize()/2.f);
	FPSText->SetParent(Camera);
	NWindow* MyWindow = GetGame()->GetScene()->AddWindow();
	MyWindow->SetTexture("window");
	MyWindow->SetScale(256,256);
	MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
	MyWindow->SetParent(Camera);
		NButton* QuitButton = GetGame()->GetScene()->AddButton();
		QuitButton->SetTexture("button");
		QuitButton->SetScale(64,32);
		QuitButton->SetText(_t("Quit"));
		QuitButton->SetPos(0,-32);
		QuitButton->SetParent(MyWindow);
		NButton* PlayButton = GetGame()->GetScene()->AddButton();
		PlayButton->SetTexture("button");
		PlayButton->SetScale(64,32);
		PlayButton->SetText(_t("Play"));
		PlayButton->SetPos(0,32);
		PlayButton->SetParent(MyWindow);
	NWindow* InfoWindow = GetGame()->GetScene()->AddWindow();
	InfoWindow->SetTexture("window");
	InfoWindow->SetScale(128,256);
	NText* InfoText = GetGame()->GetScene()->AddText("cousine", _t("This game is in very early development, but the engine framework is pretty much completed now. You can see lots of it demo'd here. Use wasd and qe to move the camera around. While escape brings back up the beginning menu."));
	InfoText->SetBorder(128,256);
	InfoText->SetPos(-64,108);
	InfoText->SetSize(12);
	InfoText->SetParent(InfoWindow);
	InfoWindow->SetParent(Camera);
	NPlayer* MyPlayer = GetGame()->GetScene()->AddPlayer();
	MyPlayer->SetControl();
	NLight* MyLight = GetGame()->GetScene()->AddLight("point");
	MyLight->SetScale(512,512);
	while(GetGame()->Running())
	{
		MyLight->SetPos(MyPlayer->GetPos());
		WantedPosition = glm::vec3(MyPlayer->GetPos()+glm::vec3(0,0,500));
		Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedPosition)/8.f);
		GetGame()->GetInput()->Poll();
		if (PlayButton->OnRelease())
		{
		    CoinSound->Play();
			PlayButton->SetColor(glm::vec4(1,1,1,0));
			QuitButton->SetColor(glm::vec4(1,1,1,0));
			MyWindow->SetColor(glm::vec4(1,1,1,0));
			GetGame()->GetMap()->Init(16,16,16);
			MyPlayer->SetPos(512,512,1024);
		}
		if (GetGame()->GetInput()->KeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
		{
			MyLight = GetGame()->GetScene()->AddLight("point");
			float Size = Rand(50,512);
			MyLight->SetScale(Size,Size);
			MyLight->SetColor(Rand(0,1),Rand(0,1),Rand(0,1), 0.5);
		}
		if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
		{
			if (MyWindow->GetColor().w == 0)
			{
				PlayButton->SetColor(glm::vec4(1,1,1,1));
				QuitButton->SetColor(glm::vec4(1,1,1,1));
				MyWindow->SetColor(glm::vec4(1,1,1,1));
			} else {
				PlayButton->SetColor(glm::vec4(1,1,1,0));
				QuitButton->SetColor(glm::vec4(1,1,1,0));
				MyWindow->SetColor(glm::vec4(1,1,1,0));
			}
		}
		MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
		InfoWindow->SetPos(GetGame()->GetWindowSize()-glm::vec2(64,128));
		std::stringstream NewText(std::stringstream::in | std::stringstream::out);
		NewText << "FPS: " << (1.f/GetGame()->GetRender()->GetFrameTime());
		FPSText->SetText(NewText.str());
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
		if (QuitButton->OnRelease())
		{
			GetGame()->Close();
		}
	}
	GetGame()->CleanUp();
	return 0;
}
