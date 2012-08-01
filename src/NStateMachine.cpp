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
        Window = new NWindow("window");
        Window->SetScale(256,256);
        Window->SetPos(GetGame()->GetWindowSize()/2.f);
        Window->SetParent(GetGame()->GetRender()->GetCamera());
        QuitButton = new NButton("button");
        QuitButton->SetScale(150,32);
        QuitButton->SetText(_t("Quit"));
        QuitButton->SetPos(0,-48);
        QuitButton->SetParent(Window);
        PlayButton = new NButton("button");
        PlayButton->SetScale(150,32);
        PlayButton->SetText(_t("Play"));
        PlayButton->SetPos(0,-16);
        PlayButton->SetParent(Window);
        MultiButton = new NButton("button");
        MultiButton->SetScale(150,32);
        MultiButton->SetText(_t("Play Online"));
        MultiButton->SetPos(0,16);
        MultiButton->SetParent(Window);
        MapButton = new NButton("button");
        MapButton->SetScale(150,32);
        MapButton->SetText(_t("Make new map"));
        MapButton->SetPos(0,48);
        MapButton->SetParent(Window);
        InfoWindow = new NWindow("window");
        InfoWindow->SetScale(128,256);
        InfoWindow->SetPos(GetGame()->GetWindowSize()-glm::vec2(64,128));
        InfoWindow->SetParent(GetGame()->GetRender()->GetCamera());
        InfoText = new NText("cousine", _t("This game is in very early development, but the engine framework is pretty much completed now. You can see lots of it demo'd here. Use wasd and qe to move the camera around. While escape brings back up the beginning menu."));
        InfoText->SetBorder(128,256);
        InfoText->SetPos(-64,108);
        InfoText->SetSize(12);
        InfoText->SetParent(InfoWindow);
        PlaySound = new NSound("coin");
        Init = true;
    }
}
void NPauseState::OnExit()
{
    if (Init)
    {
        Init = false;
        Window->Remove();
        InfoWindow->Remove();
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
        GetGame()->GetMap()->Load("default");
        std::string Name = GetGame()->GetConfig()->GetString("PlayerName");
        Player = new NPlayer(ToMBS(Name));
        Player->SetControl();
        Light = new NLight("ray");
        Light->SetScale(glm::vec3(512,350,1));
        Light->SetColor(glm::vec4(1,1,1,1));
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
                GetGame()->GetLog()->Send("SERVER",1,"Disconnected from host!");
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
                std::stringstream Message;
                Message << "Client connected from " << Buffer << " on port " << Event.peer->address.port << ".";
                GetGame()->GetLog()->Send("SERVER",2,Message.str());
                Event.peer->data = new unsigned int[1];
                *(unsigned int*)Event.peer->data = 0;
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                unsigned int* ID = (unsigned int*)Event.peer->data;
                if (ID != 0)
                {
                    NPlayer* Player = (NPlayer*)GetGame()->GetScene()->GetNodeByID(*ID);
                    GetGame()->GetLog()->Send("SERVER",2,Player->GetName() + " left the game.");
                    GetGame()->GetPacketHandler()->RemovePlayer(*ID);
                    Player->Remove();
                } else {
                    GetGame()->GetLog()->Send("SERVER",2,"A client has disconnected.");
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
                GetGame()->GetLog()->Send("SERVER",2,"Alex: I downloaded the whole packet, and they have been chasing us ever since.");
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
        Entities = GetGame()->GetEntityManager()->GetEntityNames();
        NCamera* Camera = GetGame()->GetRender()->GetCamera();
        Camera->SetPos(glm::vec3(512,512,500));
        WantedPosition = glm::vec3(512,512,500);
        GetGame()->GetMap()->Init(16,16,6);
        MapDim[0] = 16;
        MapDim[1] = 16;
        MapDim[2] = 6;
        GetGame()->GetScene()->SetFullBright(true);
        Window = new NWindow("window");
        Window->SetScale(128,150);
        Window->SetPos(64,75);
        Window->SetParent(Camera);
        OtherWindow = new NWindow("window");
        OtherWindow->SetScale(128,150);
        OtherWindow->SetParent(Camera);
        OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,75,0));
        CheckBox = new NCheckbox("checkbox");
        CheckBox->SetScale(16,16);
        CheckBox->SetPos(glm::vec3(-16,13+15,0));
        CheckBox->SetParent(OtherWindow);
        CheckText = new NText("cousine",_t("Solid"));
        CheckText->SetSize(12);
        CheckText->SetPos(glm::vec3(0,15+15,0));
        CheckText->SetParent(OtherWindow);
        OCheckBox = new NCheckbox("checkbox");
        OCheckBox->SetScale(16,16);
        OCheckBox->SetPos(glm::vec3(-16,-7+15,0));
        OCheckBox->SetParent(OtherWindow);
        OCheckText = new NText("cousine",_t("Opaque"));
        OCheckText->SetSize(12);
        OCheckText->SetPos(glm::vec3(0,-5+15,0));
        OCheckText->SetParent(OtherWindow);
        Increase = new NButton("button");
        Increase->SetScale(16,16);
        Increase->SetText(_t(">"));
        Increase->SetPos(64-16,37+15);
        Increase->SetParent(OtherWindow);
        Decrease = new NButton("button");
        Decrease->SetScale(16,16);
        Decrease->SetText(_t("<"));
        Decrease->SetPos(-64+16,37+15);
        Decrease->SetParent(OtherWindow);
        ChangingText = new NText("cousine",_t("Tile: 0"));
        ChangingText->SetSize(16);
        ChangingText->SetMode(1);
        ChangingText->SetPos(0,56);
        ChangingText->SetParent(OtherWindow);
        Text = new NText("cousine",_t("Welcome to the map editor! Use the WASD keys move and arrowkeys to change levels. You can also press F to toggle fullbright as well as QE to quick change tiles."));
        Text->SetSize(12);
        Text->SetBorder(128,150);
        Text->SetPos(-64,75-18);
        Text->SetParent(Window);
        Init = true;
        CurrentTile = 0;
        HWindow = new NWindow("highlight");
        HWindow->SetScale(128,128);
        HWindow->SetLayer(1); // Place it into the world
        HWindow->SwapView();
        Bg = new NWindow("space");
        unsigned int W = GetGame()->GetWindowWidth();
        unsigned int H = GetGame()->GetWindowHeight();
        if (W > H) 
        {
            Bg->SetScale(glm::vec3(W,W,1));
        } else {
            Bg->SetScale(glm::vec3(H,H,1));
        }
        Bg->SetLayer(0); // Place it into space.
        Bg->SetUI(false);
        Bg->SetParent(Camera);
        Bg->SetPos(GetGame()->GetWindowSize()/2.f);

        SlopeUp = new NButton("button");
        SlopeUp->SetScale(16,16);
        SlopeUp->SetText(_t("^"));
        SlopeUp->SetPos(0,-8);
        SlopeUp->SetToggleable(true);
        SlopeUp->SetParent(OtherWindow);

        SlopeDown = new NButton("button");
        SlopeDown->SetScale(16,16);
        SlopeDown->SetText(_t("v"));
        SlopeDown->SetPos(0,-40);
        SlopeDown->SetToggleable(true);
        SlopeDown->SetParent(OtherWindow);

        SlopeRight = new NButton("button");
        SlopeRight->SetScale(16,16);
        SlopeRight->SetText(_t(">"));
        SlopeRight->SetPos(16,-24);
        SlopeRight->SetToggleable(true);
        SlopeRight->SetParent(OtherWindow);

        SlopeLeft = new NButton("button");
        SlopeLeft->SetScale(16,16);
        SlopeLeft->SetText(_t("<"));
        SlopeLeft->SetPos(-16,-24);
        SlopeLeft->SetToggleable(true);
        SlopeLeft->SetParent(OtherWindow);

        SlopeOff = new NButton("button");
        SlopeOff->SetScale(16,16);
        SlopeOff->SetText(_t("o"));
        SlopeOff->SetPos(0,-24);
        SlopeOff->SetToggleable(true);
        SlopeOff->SetParent(OtherWindow);
    }
    SaveWindow = NULL;
}
void NMapState::OnExit()
{
    GetGame()->GetScene()->RemoveByType(NodeStar);
    if (Init)
    {
        GetGame()->GetMap()->DeInit();
        HWindow->Remove();
        Window->Remove();
        OtherWindow->Remove();
        Bg->Remove();
        if (SaveWindow)
        {
            SaveWindow->Remove();
        }
        Init = false;
    }
}
void NMapState::Tick(double DT)
{
    if (SlopeUp->OnPressed() && SlopeUp->GetToggle())
    {
        SlopeDown->SetToggle(false);
        SlopeRight->SetToggle(false);
        SlopeLeft->SetToggle(false);
        SlopeOff->SetToggle(false);
    }
    if (SlopeLeft->OnPressed() && SlopeLeft->GetToggle())
    {
        SlopeUp->SetToggle(false);
        SlopeDown->SetToggle(false);
        SlopeRight->SetToggle(false);
        SlopeOff->SetToggle(false);
    }
    if (SlopeRight->OnPressed() && SlopeRight->GetToggle())
    {
        SlopeUp->SetToggle(false);
        SlopeDown->SetToggle(false);
        SlopeLeft->SetToggle(false);
        SlopeOff->SetToggle(false);
    }
    if (SlopeDown->OnPressed() && SlopeDown->GetToggle())
    {
        SlopeUp->SetToggle(false);
        SlopeRight->SetToggle(false);
        SlopeLeft->SetToggle(false);
        SlopeOff->SetToggle(false);
    }
    if (SlopeOff->OnPressed() && SlopeOff->GetToggle())
    {
        SlopeUp->SetToggle(false);
        SlopeRight->SetToggle(false);
        SlopeLeft->SetToggle(false);
        SlopeDown->SetToggle(false);
    }
    unsigned int W = GetGame()->GetWindowWidth();
    unsigned int H = GetGame()->GetWindowHeight();
    if (W > H) 
    {
        Bg->SetScale(glm::vec3(W,W,1));
    } else {
        Bg->SetScale(glm::vec3(H,H,1));
    }
    Bg->SetPos(GetGame()->GetWindowSize()/2.f);
    std::vector<NNode*>* Stars = GetGame()->GetScene()->GetLayer(0);
    while(Stars->size()<(W+H)/20)
    {
        new NStar();
    }
    OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,50,0));
    if (Increase->OnRelease() || (GetGame()->GetInput()->KeyChanged('E') && GetGame()->GetInput()->GetKey('E')))
    {
        CurrentTile++;
        if (CurrentTile>int(GetGame()->GetMap()->GetTileCount()))
        {
            CurrentTile = -Entities.size();
        }
        if (CurrentTile >= 0)
        {
            std::wstringstream Stream;
            Stream << "Tile: ";
            Stream << CurrentTile;
            ChangingText->SetText(Stream.str());
            NTile Tile(CurrentTile);
            CheckBox->SetCheck(Tile.IsSolid());
            OCheckBox->SetCheck(Tile.IsOpaque());
        } else {
            ChangingText->SetText(Entities[-CurrentTile-1]);
            CheckBox->SetCheck(false);
            OCheckBox->SetCheck(false);
        }
    }
    if (Decrease->OnRelease() || (GetGame()->GetInput()->KeyChanged('Q') && GetGame()->GetInput()->GetKey('Q')))
    {
        CurrentTile--;
        if (CurrentTile<-int(Entities.size()))
        {
            CurrentTile = GetGame()->GetMap()->GetTileCount();
        }
        if (CurrentTile >= 0)
        {
            std::wstringstream Stream;
            Stream << "Tile: ";
            Stream << CurrentTile;
            ChangingText->SetText(Stream.str());
            NTile Tile(CurrentTile);
            CheckBox->SetCheck(Tile.IsSolid());
            OCheckBox->SetCheck(Tile.IsOpaque());
        } else {
            ChangingText->SetText(Entities[-CurrentTile-1]);
            CheckBox->SetCheck(false);
            OCheckBox->SetCheck(false);
        }
    }
    if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
    {
        if (SaveWindow)
        {
            SaveWindow->Remove();
            SaveWindow = NULL;
        } else {
            SaveWindow = new NWindow("window");
            SaveWindow->SetScale(256,256);
            SaveWindow->SetPos(GetGame()->GetWindowSize()/2.f);
            SaveWindow->SetParent(GetGame()->GetRender()->GetCamera());
            Input = new NTextInput("textbox");
            Input->SetScale(200,24);
            Input->SetPos(0,100);
            Input->GetText()->SetMultiline(false);
            Input->SetText(_t("MapName"));
            Input->SetParent(SaveWindow);
            SaveButton = new NButton("button");
            SaveButton->SetScale(64,32);
            SaveButton->SetText(_t("Save"));
            SaveButton->SetPos(0,62);
            SaveButton->SetParent(SaveWindow);
            LoadButton = new NButton("button");
            LoadButton->SetScale(64,32);
            LoadButton->SetText(_t("Load"));
            LoadButton->SetPos(0,30);
            LoadButton->SetParent(SaveWindow);
            QuitButton = new NButton("button");
            QuitButton->SetScale(64,32);
            QuitButton->SetText(_t("Quit"));
            QuitButton->SetPos(0,-2);
            QuitButton->SetParent(SaveWindow);
            MapIncrease[0] = new NButton("button");
            MapIncrease[0]->SetScale(16,16);
            MapIncrease[0]->SetPos(64,-64);
            MapIncrease[0]->SetText(_t(">"));
            MapIncrease[0]->SetParent(SaveWindow);
            MapIncrease[1] = new NButton("button");
            MapIncrease[1]->SetScale(16,16);
            MapIncrease[1]->SetPos(64,-82);
            MapIncrease[1]->SetText(_t(">"));
            MapIncrease[1]->SetParent(SaveWindow);
            MapIncrease[2] = new NButton("button");
            MapIncrease[2]->SetScale(16,16);
            MapIncrease[2]->SetPos(64,-100);
            MapIncrease[2]->SetText(_t(">"));
            MapIncrease[2]->SetParent(SaveWindow);

            MapDecrease[0] = new NButton("button");
            MapDecrease[0]->SetScale(16,16);
            MapDecrease[0]->SetPos(-64,-64);
            MapDecrease[0]->SetText(_t("<"));
            MapDecrease[0]->SetParent(SaveWindow);
            MapDecrease[1] = new NButton("button");
            MapDecrease[1]->SetScale(16,16);
            MapDecrease[1]->SetPos(-64,-82);
            MapDecrease[1]->SetText(_t("<"));
            MapDecrease[1]->SetParent(SaveWindow);
            MapDecrease[2] = new NButton("button");
            MapDecrease[2]->SetScale(16,16);
            MapDecrease[2]->SetPos(-64,-100);
            MapDecrease[2]->SetText(_t("<"));
            MapDecrease[2]->SetParent(SaveWindow);

            MapDim[0] = GetGame()->GetMap()->GetWidth();
            MapDim[1] = GetGame()->GetMap()->GetHeight();
            MapDim[2] = GetGame()->GetMap()->GetDepth();
            std::wstringstream String;
            String << "Width: " << MapDim[0];
            Dimensions[0] = new NText("cousine",String.str());
            Dimensions[0]->SetPos(0,-64);
            Dimensions[0]->SetSize(16);
            Dimensions[0]->SetMode(1);
            Dimensions[0]->SetParent(SaveWindow);
            std::wstringstream Stringh;
            Stringh << "Height: " << MapDim[1];
            Dimensions[1] = new NText("cousine",Stringh.str());
            Dimensions[1]->SetPos(0,-82);
            Dimensions[1]->SetSize(16);
            Dimensions[1]->SetMode(1);
            Dimensions[1]->SetParent(SaveWindow);
            std::wstringstream Stringd;
            Stringd << "Depth: " << MapDim[2];
            Dimensions[2] = new NText("cousine",Stringd.str());
            Dimensions[2]->SetPos(0,-100);
            Dimensions[2]->SetSize(16);
            Dimensions[2]->SetMode(1);
            Dimensions[2]->SetParent(SaveWindow);
            Apply = new NButton("button");
            Apply->SetText(_t("Apply"));
            Apply->GetText()->SetSize(14);
            Apply->SetScale(64,16);
            Apply->SetPos(0,-120);
            Apply->SetParent(SaveWindow);
            NText* TempText = new NText("cousine",_t("Map dimensions"));
            TempText->SetMode(1);
            TempText->SetPos(0,-34);
            TempText->SetSize(16);
            TempText->SetParent(SaveWindow);
        }
    }
    if (SaveWindow)
    {
        SaveWindow->SetPos(GetGame()->GetWindowSize()/2.f);
        if (SaveButton->OnRelease())
        {
            GetGame()->GetMap()->Save(ToMBS(Input->GetEnteredText()));
        }
        if (LoadButton->OnRelease())
        {
            GetGame()->GetMap()->Load(ToMBS(Input->GetEnteredText()));
        }
        if (QuitButton->OnRelease())
        {
            GetGame()->GetStateMachine()->SetState("Paused");
            return;
        }
        if (Apply->OnRelease())
        {
            GetGame()->GetMap()->Init(MapDim[0],MapDim[1],MapDim[2]);
        }
        for (unsigned int i=0;i<3;i++)
        {
            if (MapDecrease[i]->OnRelease())
            {
                MapDim[i]--;
                if (MapDim[i] < 0)
                {
                    MapDim[i] = 0;
                }
                std::wstringstream Stream;
                switch(i)
                {
                    case 0: Stream << "Width: "; break;
                    case 1: Stream << "Height: "; break;
                    case 2: Stream << "Depth: "; break;
                }
                Stream << MapDim[i];
                Dimensions[i]->SetText(Stream.str());
            }
            if (MapIncrease[i]->OnRelease())
            {
                MapDim[i]++;
                std::wstringstream Stream;
                switch(i)
                {
                    case 0: Stream << "Width: "; break;
                    case 1: Stream << "Height: "; break;
                    case 2: Stream << "Depth: "; break;
                }
                Stream << MapDim[i];
                Dimensions[i]->SetText(Stream.str());
            }
        }
    }
    if (GetGame()->GetInput()->KeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
    {
        GetGame()->GetScene()->ToggleFullBright();
    }
    if (!GetGame()->GetScene()->GetFullBright())
    {
        HWindow->SetColor(glm::vec4(0,0,0,0));
    } else {
        HWindow->SetColor(glm::vec4(1,1,1,1));
    }
    NCamera* Camera = GetGame()->GetRender()->GetCamera();
    float Multi = DT*5;
    if (Multi <= 1)
    {
        Camera->SetPos(Camera->GetPos()+((WantedPosition-Camera->GetPos()))*float(DT*5));
    } else {
        Camera->SetPos(WantedPosition);
    }
    if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_UP) && GetGame()->GetInput()->GetKey(GLFW_KEY_UP))
    {
        if (GetGame()->GetMap()->GetLevel(WantedPosition-glm::vec3(0,0,500)) < GetGame()->GetMap()->GetDepth()-1)
        {
            WantedPosition += glm::vec3(0,0,GetGame()->GetMap()->GetTileSize());
        }
    }
    if (GetGame()->GetInput()->KeyChanged(GLFW_KEY_DOWN) && GetGame()->GetInput()->GetKey(GLFW_KEY_DOWN))
    {
        if (GetGame()->GetMap()->GetLevel(WantedPosition-glm::vec3(0,0,500)) > 0)
        {
            WantedPosition -= glm::vec3(0,0,GetGame()->GetMap()->GetTileSize());
        }
    }
    if (GetGame()->GetInput()->GetKey('W'))
    {
        WantedPosition += glm::vec3(0,500,0)*float(DT);
    }
    if (GetGame()->GetInput()->GetKey('S'))
    {
        WantedPosition -= glm::vec3(0,500,0)*float(DT);
    }
    if (GetGame()->GetInput()->GetKey('A'))
    {
        WantedPosition -= glm::vec3(500,0,0)*float(DT);
    }
    if (GetGame()->GetInput()->GetKey('D'))
    {
        WantedPosition += glm::vec3(500,0,0)*float(DT);
    }
    glm::vec3 TilePos = GetGame()->GetMap()->TilePos(GetGame()->GetInput()->GetPerspMouse(.45));
    HWindow->SetPos(TilePos);
    if (GetGame()->GetInput()->GetMouseKey(0) && !GetGame()->GetInput()->GetMouseHitGUI())
    {
        if (CurrentTile >= 0)
        {
            NTile* Tile = GetGame()->GetMap()->GetTile(GetGame()->GetInput()->GetPerspMouse(.45));
            if (Tile)
            {
                Tile->SetID(CurrentTile);
                Tile->SetSolid(CheckBox->IsChecked());
                Tile->SetOpaque(OCheckBox->IsChecked());
                if (SlopeUp->GetToggle())
                {
                    Tile->SetSlope(SlopeNorth);
                } else if (SlopeDown->GetToggle())
                {
                    Tile->SetSlope(SlopeSouth);
                } else if (SlopeRight->GetToggle())
                {
                    Tile->SetSlope(SlopeEast);
                } else if (SlopeLeft->GetToggle())
                {
                    Tile->SetSlope(SlopeWest);
                } else {
                    SlopeOff->SetToggle(true);
                    Tile->SetSlope(SlopeNone);
                }
            }
        } else {
            if (GetGame()->GetInput()->KeyChanged(0))
            {
                NEntity* Entity = new NEntity(Entities[-CurrentTile-1],GetGame()->GetInput()->GetPerspMouse(.45));
            }
        }
    }
    GetGame()->GetInput()->KeyChanged(0);
}
std::string NMapState::GetName()
{
    return "Mapping";
}
