#include "NEngine.hpp"

// --- State Template ---
NState::NState()
{
	Init = false;
}
NState::~NState() {}
void NState::Tick(double DT) {}
std::string NState::GetName()
{
	return "NULL";
}
void NState::OnExit() {}
void NState::OnEnter() {}

// --- State Machine ---
NStateMachine::NStateMachine(std::string State)
{
	CurrentState = -1;
	LastExecution = CurTime();
	States.push_back(new NPauseState());
	States.push_back(new NGameState());
	States.push_back(new NOnlineState());
	States.push_back(new NServerState());
	States.push_back(new NMapState());
	SetState(State);
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
			if (CurrentState != -1)
			{
				States[CurrentState]->OnExit();
			}
			CurrentState = i;
			States[CurrentState]->OnEnter();
			break;
		}
	}
}
void NStateMachine::Tick()
{
	if (CurrentState == -1)
	{
		return;
	}
	double DT = CurTime()-LastExecution;
	States[CurrentState]->Tick(DT);
	LastExecution = CurTime();
}

// --- Pause State ---
NPauseState::NPauseState()
{
}
NPauseState::~NPauseState()
{
}
void NPauseState::OnEnter()
{
	if (!Init)
	{
		Window = GetGame()->GetScene()->AddWindow();
		Window->SetTexture("window");
		Window->SetScale(256,256);
		Window->SetPos(GetGame()->GetWindowSize()/2.f);
		Window->SetParent(GetGame()->GetRender()->GetCamera());
		QuitButton = GetGame()->GetScene()->AddButton();
		QuitButton->SetTexture("button");
		QuitButton->SetScale(150,32);
		QuitButton->SetText(_t("Quit"));
		QuitButton->SetPos(0,-48);
		QuitButton->SetParent(Window);
		PlayButton = GetGame()->GetScene()->AddButton();
		PlayButton->SetTexture("button");
		PlayButton->SetScale(150,32);
		PlayButton->SetText(_t("Play"));
		PlayButton->SetPos(0,-16);
		PlayButton->SetParent(Window);
		MultiButton = GetGame()->GetScene()->AddButton();
		MultiButton->SetTexture("button");
		MultiButton->SetScale(150,32);
		MultiButton->SetText(_t("Play Online"));
		MultiButton->SetPos(0,16);
		MultiButton->SetParent(Window);
		MapButton = GetGame()->GetScene()->AddButton();
		MapButton->SetTexture("button");
		MapButton->SetScale(150,32);
		MapButton->SetText(_t("Make new map"));
		MapButton->SetPos(0,48);
		MapButton->SetParent(Window);
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
		Init = true;
	}
}
void NPauseState::OnExit()
{
	if (Init)
	{
		Init = false;
		GetGame()->GetScene()->Remove(Window);
		GetGame()->GetScene()->Remove(InfoWindow);
	}
}
void NPauseState::Tick(double DT)
{
	Window->SetPos(GetGame()->GetWindowSize()/2.f);
	InfoWindow->SetPos(GetGame()->GetWindowSize()-glm::vec2(64,128));
	if (PlayButton->OnRelease())
	{
		PlaySound->Play();
		GetGame()->GetStateMachine()->SetState("Game");
		return;
	}
	if (QuitButton->OnRelease())
	{
		PlaySound->Play();
		GetGame()->Close();
	}
	if (MultiButton->OnRelease())
	{
		GetGame()->GetNetwork()->CreateClient();
		if (GetGame()->GetNetwork()->Connect("10.0.1.5",51313) == Success)
		{
			PlaySound->Play();
			NPacket Packet(GetGame()->GetConfig()->GetString("PlayerName"));
			Packet.Broadcast();
			GetGame()->GetStateMachine()->SetState("Online");
			return;
		}
	}
	if (MapButton->OnRelease())
	{
		PlaySound->Play();
		GetGame()->GetStateMachine()->SetState("Mapping");
		return;
	}
	if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
	{
		GetGame()->GetStateMachine()->SetState("Game");
		return;
	}
}
std::string NPauseState::GetName()
{
	return "Paused";
}

// --- Game State ---
NGameState::NGameState()
{
}
NGameState::~NGameState()
{
}
void NGameState::OnEnter()
{
	if (!Init)
	{
		std::string Name = GetGame()->GetConfig()->GetString("PlayerName");
		Player = GetGame()->GetScene()->AddPlayer(ToMBS(Name));
		Player->SetControl();
		Light = GetGame()->GetScene()->AddLight("ray");
		Light->SetScale(glm::vec3(512,350,1));
		Light->SetColor(glm::vec4(1,1,1,1));
		GetGame()->GetMap()->Init(16,16,16);
		Player->SetPos(512,512,1024);
		Init = true;
	}
}
void NGameState::OnExit()
{
}
void NGameState::Tick(double DT)
{
	NCamera* Camera = GetGame()->GetRender()->GetCamera();
	glm::vec3 Pos = glm::vec3(GetGame()->GetInput()->GetMouseX(),GetGame()->GetInput()->GetMouseY(),0)-Player->GetPos()-glm::vec3(GetGame()->GetWindowWidth()/2.f,GetGame()->GetWindowHeight()/2.f,0);
	Light->SetAng(glm::vec3(0,0,90-(atan2(Pos.x,Pos.y)/PI)*180));
	glm::vec3 WantedCameraPos = Player->GetPos()+glm::vec3(0,0,500);
	Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedCameraPos)/float(1.f+DT*50.f));
	Camera->SetPos(glm::vec3(Camera->GetPos().x,Camera->GetPos().y,WantedCameraPos.z));
	Light->SetPos(Player->GetPos());
	if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
	{
		GetGame()->GetStateMachine()->SetState("Paused");
	}
	if (GetGame()->GetInput()->KeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
	{
		if (Light->GetColor().w == 0)
		{
			Light->SetColor(glm::vec4(1,1,1,1));
		} else {
			Light->SetColor(glm::vec4(1,1,1,0));
		}
	}
}
std::string NGameState::GetName()
{
	return "Game";
}

// --- Online State ---
NOnlineState::NOnlineState()
{
}
NOnlineState::~NOnlineState()
{
}
void NOnlineState::OnEnter()
{
	if (!Init)
	{
		GetGame()->GetMap()->Init(16,16,16);
		Init = true;
	}
}
void NOnlineState::OnExit()
{
}
void NOnlineState::Tick(double DT)
{
	NCamera* Camera = GetGame()->GetRender()->GetCamera();
	NPlayer* Player = GetGame()->GetPacketHandler()->GetPlayer();
	if (Player != NULL)
	{
		glm::vec3 WantedCameraPos = Player->GetPos()+glm::vec3(0,0,500);
		Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedCameraPos)/float(1.f+DT*50.f));
		Camera->SetPos(glm::vec3(Camera->GetPos().x,Camera->GetPos().y,WantedCameraPos.z));
		NPacket Packet(Player);
		Packet.Broadcast();
	}
	ENetEvent Event;
	while (GetGame()->GetNetwork()->PollEvent(&Event))
	{
		switch (Event.type)
		{
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				SetColor(Yellow);
				std::cout << "SERVER WARN: ";
				ClearColor();
				std::cout << "Disconnected from host!\n";
				GetGame()->GetStateMachine()->SetState("Paused");
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::string Data;
				Data.append((const char*)Event.packet->data,Event.packet->dataLength);
				NPacket Packet(Data,Event);
				Packet.Merge();
				enet_packet_destroy(Event.packet);
				break;
			}
		}
	}
	if (GetGame()->GetInput()->KeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
	{
		GetGame()->GetScene()->ToggleFullBright();
	}
}
std::string NOnlineState::GetName()
{
	return "Online";
}

// --- Server State ---
NServerState::NServerState()
{
}
NServerState::~NServerState()
{
}
void NServerState::OnEnter()
{
	if (!Init)
	{
		GetGame()->GetMap()->Init(16,16,16);
		Init = true;
	}
}
void NServerState::OnExit()
{
}
void NServerState::Tick(double DT)
{
	NPacket* Packet = new NPacket();
	Packet->Broadcast();
	delete Packet;
	ENetEvent Event;
	while (GetGame()->GetNetwork()->PollEvent(&Event))
	{
		switch (Event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				char Buffer[64];
				enet_address_get_host_ip(&Event.peer->address,Buffer,64);
				SetColor(Blue);
				std::cout << "SERVER INFO: ";
				ClearColor();
				std::cout << "Client connected from " << Buffer << " on port " << Event.peer->address.port << ".\n";
				Event.peer->data = new unsigned int[1];
				*(unsigned int*)Event.peer->data = 0;
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				unsigned int* ID = (unsigned int*)Event.peer->data;
				if (ID != 0)
				{
					SetColor(Blue);
					std::cout << "SERVER INFO: ";
					ClearColor();
					NPlayer* Player = (NPlayer*)GetGame()->GetScene()->GetNodeByID(*ID);
					std::cout << Player->GetName() << " left the game.\n";
					GetGame()->GetPacketHandler()->RemovePlayer(*ID);
					GetGame()->GetScene()->Remove(Player);
				} else {
					SetColor(Blue);
					std::cout << "SERVER INFO: ";
					ClearColor();
					std::cout << "A client has disconnected.\n";
				}
				delete[] (unsigned int*)Event.peer->data;
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::string Data;
				Data.append((const char*)Event.packet->data,Event.packet->dataLength);
				NPacket Packet(Data,Event);
				Packet.Merge();
				enet_packet_destroy(Event.packet);
				break;
			}
			case ENET_EVENT_TYPE_NONE:
			{
				SetColor(Blue);
				std::cout << "SERVER INFO: ";
				ClearColor();
				std::cout << "aw damn we still got nothin'...\n";
				glfwSleep(1);
				break;
			}
		}
	}
	glfwSleep(1.f/60.f);
}
std::string NServerState::GetName()
{
	return "Serving";
}

// --- Map State ---
NMapState::NMapState()
{
}
NMapState::~NMapState()
{
}
void NMapState::OnEnter()
{
	if (!Init)
	{
		NCamera* Camera = GetGame()->GetRender()->GetCamera();
		Camera->SetPos(glm::vec3(1024,1024,500));
		GetGame()->GetMap()->Init(32,32,6);
		GetGame()->GetScene()->ToggleFullBright();
		Window = GetGame()->GetScene()->AddWindow();
		Window->SetTexture("window");
		Window->SetScale(128,128);
		Window->SetPos(64,64);
		Window->SetParent(Camera);
		OtherWindow = GetGame()->GetScene()->AddWindow();
		OtherWindow->SetTexture("window");
		OtherWindow->SetScale(128,80);
		OtherWindow->SetParent(Camera);
		OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,40,0));
		CheckBox = GetGame()->GetScene()->AddCheckbox("checkbox");
		CheckBox->SetScale(16,16);
		CheckBox->SetPos(glm::vec3(-16,-12,0));
		CheckBox->SetParent(OtherWindow);
		CheckText = GetGame()->GetScene()->AddText("cousine",_t("Solid"));
		CheckText->SetSize(12);
		CheckText->SetPos(glm::vec3(20,3,0));
		CheckText->SetParent(CheckBox);
		Increase = GetGame()->GetScene()->AddButton();
		Increase->SetTexture("button");
		Increase->SetScale(16,16);
		Increase->SetText(_t(">"));
		Increase->SetPos(64-16,12);
		Increase->SetParent(OtherWindow);
		Decrease = GetGame()->GetScene()->AddButton();
		Decrease->SetTexture("button");
		Decrease->SetScale(16,16);
		Decrease->SetText(_t("<"));
		Decrease->SetPos(-64+16,12);
		Decrease->SetParent(OtherWindow);
		ChangingText = GetGame()->GetScene()->AddText("cousine",_t("Tile: 0"));
		ChangingText->SetSize(16);
		ChangingText->SetMode(1);
		ChangingText->SetPos(0,16);
		ChangingText->SetParent(OtherWindow);
		Text = GetGame()->GetScene()->AddText("cousine",_t("Welcome to the map editor! Use the WASD keys to move on the current level, and QE to change levels. You can also press F to toggle fullbright."));
		Text->SetSize(12);
		Text->SetBorder(128,128);
		Text->SetPos(0,110);
		Text->SetParent(Camera);
		Init = true;
		CurrentTile = 0;
	}
}
void NMapState::OnExit()
{
}
void NMapState::Tick(double DT)
{
	OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,32,0));
	if (Increase->OnRelease())
	{
		CurrentTile++;
		if (CurrentTile>2)
		{
			CurrentTile = 0;
		}
		std::wstringstream Stream;
		Stream << "Tile: ";
		Stream << CurrentTile;
		ChangingText->SetText(Stream.str());
		NTile Tile(CurrentTile);
		CheckBox->SetCheck(Tile.IsSolid());
	}
	if (Decrease->OnRelease())
	{
		CurrentTile--;
		if (CurrentTile<0)
		{
			CurrentTile = 2;
		}
		std::wstringstream Stream; Stream << "Tile: ";
		Stream << CurrentTile;
		ChangingText->SetText(Stream.str());
		NTile Tile(CurrentTile);
		CheckBox->SetCheck(Tile.IsSolid());
	}
	NCamera* Camera = GetGame()->GetRender()->GetCamera();
	if (GetGame()->GetInput()->KeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
	{
		GetGame()->GetScene()->ToggleFullBright();
	}
	if (GetGame()->GetInput()->KeyChanged('Q') && GetGame()->GetInput()->GetKey('Q'))
	{
		if (GetGame()->GetMap()->GetLevel() < GetGame()->GetMap()->GetDepth()-1)
		{
			Camera->SetPos(Camera->GetPos()+glm::vec3(0,0,GetGame()->GetMap()->GetTileSize()));
		}
	}
	if (GetGame()->GetInput()->KeyChanged('E') && GetGame()->GetInput()->GetKey('E'))
	{
		if (GetGame()->GetMap()->GetLevel() > 0)
		{
			Camera->SetPos(Camera->GetPos()-glm::vec3(0,0,GetGame()->GetMap()->GetTileSize()));
		}
	}
	if (GetGame()->GetInput()->GetKey('W'))
	{
		Camera->SetPos(Camera->GetPos()+glm::vec3(0,200,0)*float(DT));
	}
	if (GetGame()->GetInput()->GetKey('S'))
	{
		Camera->SetPos(Camera->GetPos()-glm::vec3(0,200,0)*float(DT));
	}
	if (GetGame()->GetInput()->GetKey('A'))
	{
		Camera->SetPos(Camera->GetPos()-glm::vec3(200,0,0)*float(DT));
	}
	if (GetGame()->GetInput()->GetKey('D'))
	{
		Camera->SetPos(Camera->GetPos()+glm::vec3(200,0,0)*float(DT));
	}
	if (GetGame()->GetInput()->GetMouseKey(0) && !GetGame()->GetInput()->GetMouseHitGUI())
	{
		NTile* Tile = GetGame()->GetMap()->GetTile(GetGame()->GetInput()->GetPerspMouse(.45));
		if (Tile)
		{
			Tile->SetID(CurrentTile);
			Tile->SetSolid(CheckBox->IsChecked());
		}
	}
}
std::string NMapState::GetName()
{
	return "Mapping";
}
