#include "NEngine.hpp"

// --- State Template ---
NState::NState() {}
NState::~NState() {}
void NState::Tick(double DT) {}
std::string NState::GetName()
{
	return "NULL";
}
void NState::OnExit() {}
void NState::OnEnter() {}

// --- State Machine ---
NStateMachine::NStateMachine()
{
	CurrentState = 0;
	LastExecution = CurTime();
	States.push_back(new NPauseState());
	States.push_back(new NGameState());
}
NStateMachine::~NStateMachine() 
{
	for (unsigned int i=0;i<States.size();i++)
	{
		delete States[i];
	}
}
void NStateMachine::SetState(std::string Name)
{
	for (unsigned int i=0;i<States.size();i++)
	{
		if (States[i]->GetName() == Name)
		{
			States[CurrentState]->OnExit();
			CurrentState = i;
			States[CurrentState]->OnEnter();
			break;
		}
	}
}
void NStateMachine::Tick()
{
	double DT = CurTime()-LastExecution;
	States[CurrentState]->Tick(DT);
	LastExecution = CurTime();
}

// --- Pause State ---
NPauseState::NPauseState()
{
	Window = GetGame()->GetScene()->AddWindow();
	Window->SetTexture("window");
	Window->SetScale(256,256);
	Window->SetPos(GetGame()->GetWindowSize()/2.f);
	Window->SetParent(GetGame()->GetRender()->GetCamera());
	QuitButton = GetGame()->GetScene()->AddButton();
	QuitButton->SetTexture("button");
	QuitButton->SetScale(64,32);
	QuitButton->SetText(_t("Quit"));
	QuitButton->SetPos(0,-32);
	QuitButton->SetParent(Window);
	PlayButton = GetGame()->GetScene()->AddButton();
	PlayButton->SetTexture("button");
	PlayButton->SetScale(64,32);
	PlayButton->SetText(_t("Play"));
	PlayButton->SetPos(0,32);
	PlayButton->SetParent(Window);
	InfoWindow = GetGame()->GetScene()->AddWindow();
	InfoWindow->SetTexture("window");
	InfoWindow->SetScale(128,256);
	InfoText = GetGame()->GetScene()->AddText("cousine", _t("This game is in very early development, but the engine framework is pretty much completed now. You can see lots of it demo'd here. Use wasd and qe to move the camera around. While escape brings back up the beginning menu."));
	InfoText->SetBorder(128,256);
	InfoText->SetPos(-64,108);
	InfoText->SetSize(12);
	InfoText->SetParent(InfoWindow);
	InfoWindow->SetParent(GetGame()->GetRender()->GetCamera());
	PlaySound = GetGame()->GetScene()->AddSound("coin");
}
NPauseState::~NPauseState()
{
}
void NPauseState::OnEnter()
{
	Window->SetColor(glm::vec4(1,1,1,1));
	QuitButton->SetColor(glm::vec4(1,1,1,1));
	PlayButton->SetColor(glm::vec4(1,1,1,1));
	InfoWindow->SetColor(1,1,1,1);
	InfoText->SetColor(1,1,1,1);
}
void NPauseState::OnExit()
{
	Window->SetColor(glm::vec4(1,1,1,0));
	QuitButton->SetColor(glm::vec4(1,1,1,0));
	PlayButton->SetColor(glm::vec4(1,1,1,0));
	InfoWindow->SetColor(1,1,1,0);
	InfoText->SetColor(1,1,1,0);
}
void NPauseState::Tick(double DT)
{
	Window->SetPos(GetGame()->GetWindowSize()/2.f);
	InfoWindow->SetPos(GetGame()->GetWindowSize()-glm::vec2(64,128));
	if (PlayButton->OnRelease())
	{
		PlaySound->Play();
		GetGame()->GetStateMachine()->SetState("Game");
	}
	if (QuitButton->OnRelease())
	{
		GetGame()->Close();
	}
	if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
	{
		GetGame()->GetStateMachine()->SetState("Game");
	}
}
std::string NPauseState::GetName()
{
	return "Paused";
}

// --- Game State ---
NGameState::NGameState()
{
	std::string Name = GetGame()->GetConfig()->GetString("PlayerName");
	Player = GetGame()->GetScene()->AddPlayer(ToMBS(Name));
	Player->SetControl();
	Light = GetGame()->GetScene()->AddLight("point");
	Light->SetScale(glm::vec3(512,512,1));
	GetGame()->GetMap()->Init(16,16,16);
	Player->SetPos(512,512,1024);
}
NGameState::~NGameState()
{
}
void NGameState::OnEnter()
{
}
void NGameState::OnExit()
{
}
void NGameState::Tick(double DT)
{
	NCamera* Camera = GetGame()->GetRender()->GetCamera();
	glm::vec3 WantedCameraPos = Player->GetPos()+glm::vec3(0,0,500);
	Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedCameraPos)/float(1.f+DT*50.f));
	Camera->SetPos(glm::vec3(Camera->GetPos().x,Camera->GetPos().y,WantedCameraPos.z));
	Light->SetPos(Player->GetPos());
	if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
	{
		GetGame()->GetStateMachine()->SetState("Paused");
	}
}
std::string NGameState::GetName()
{
	return "Game";
}
