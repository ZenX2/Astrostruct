#include "NEngine.hpp"

// --- State Template ---
NState::NState()
{
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
    Window = new NWindow("window");
    Window->SetScale(256,256);
    glm::vec2 WSize = GetGame()->GetWindowSize()/2.f;
    Window->SetPos(glm::vec3(WSize.x,WSize.y,0));
    Window->SetParent(GetGame()->GetRender()->GetCamera());
    QuitButton = new NButton("button");
    QuitButton->SetScale(150,32);
    QuitButton->SetText(GetGame()->GetLua()->Translate(UIQuit));
    QuitButton->SetPos(glm::vec3(0,-48,0));
    QuitButton->SetShrinkWrap(true);
    QuitButton->SetParent(Window);
    PlayButton = new NButton("button");
    PlayButton->SetScale(150,32);
    PlayButton->SetText(GetGame()->GetLua()->Translate(UIPlay));
    PlayButton->SetPos(glm::vec3(0,-16,0));
    PlayButton->SetShrinkWrap(true);
    PlayButton->SetParent(Window);
    MultiButton = new NButton("button");
    MultiButton->SetScale(150,32);
    MultiButton->SetText(GetGame()->GetLua()->Translate(UIPlayOnline));
    MultiButton->SetPos(glm::vec3(0,16,0));
    MultiButton->SetShrinkWrap(true);
    MultiButton->SetParent(Window);
    MapButton = new NButton("button");
    MapButton->SetScale(150,32);
    MapButton->SetText(GetGame()->GetLua()->Translate(UIMakeMap));
    MapButton->SetPos(glm::vec3(0,48,0));
    MapButton->SetShrinkWrap(true);
    MapButton->SetParent(Window);
    InfoWindow = new NWindow("window");
    InfoWindow->SetScale(128,256);
    InfoWindow->SetParent(GetGame()->GetRender()->GetCamera());
    glm::vec2 Temp = GetGame()->GetWindowSize()-glm::vec2(64,128);
    InfoWindow->SetPos(glm::vec3(Temp.x,Temp.y,0));
    InfoText = new NText("gui", GetGame()->GetLua()->Translate(UIIntro));
    InfoText->SetBorder(128,256);
    InfoText->SetPos(glm::vec3(-64,108,0));
    InfoText->SetSize(15);
    InfoText->SetParent(InfoWindow);
    PlaySound = new NSound("click");
}
void NPauseState::OnExit()
{
    Window->Remove();
    InfoWindow->Remove();
}
void NPauseState::Tick(double DT)
{
    glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
    Window->SetPos(glm::vec3(Temp.x,Temp.y,0));
    Temp = GetGame()->GetWindowSize()-glm::vec2(64,128);
    InfoWindow->SetPos(glm::vec3(Temp.x,Temp.y,0));
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
        if (!GetGame()->GetNetwork()->Connect("10.0.1.5",51313))
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
    GetGame()->GetScene()->SetFullBright(false);
    std::string Map = GetGame()->GetConfig()->GetString("Map");
    GetGame()->GetMap()->Load(Map);
    std::string Name = GetGame()->GetConfig()->GetString("PlayerName");
    Player = new NPlayer(ToMBS(Name));
    Player->SetControl();
    Window = NULL;
}
void NGameState::OnExit()
{
    Player->Remove();
    if (Window)
    {
        Window->Remove();
    }
    GetGame()->GetMap()->DeInit();
}
void NGameState::Tick(double DT)
{
    NCamera* Camera = GetGame()->GetRender()->GetCamera();
    //glm::vec3 Pos = glm::vec3(GetGame()->GetInput()->GetMouseX(),GetGame()->GetInput()->GetMouseY(),0)-Player->GetPos()-glm::vec3(GetGame()->GetWindowWidth()/2.f,GetGame()->GetWindowHeight()/2.f,0);
    glm::vec3 WantedCameraPos = Player->GetPos()+glm::vec3(0,0,500)-glm::vec3(0,0,GetGame()->GetMap()->GetTileSize()/2.f);
    Camera->SetPos(Camera->GetPos()-(Camera->GetPos()-WantedCameraPos)/float(1.f+DT*50.f));
    Camera->SetPos(glm::vec3(Camera->GetPos().x,Camera->GetPos().y,WantedCameraPos.z));
    if (GetGame()->GetInput()->GetKeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
    {
        if (!Window)
        {
            Window = new NWindow("window");
            Window->SetScale(256,256);
            glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
            Window->SetPos(glm::vec3(Temp.x,Temp.y,0));
            Window->SetParent(GetGame()->GetRender()->GetCamera());
            Button = new NButton("button");
            Button->SetScale(64,32);
            Button->SetText(GetGame()->GetLua()->Translate(UIQuit));
            Button->SetPos(glm::vec3(0,0,0));
            Button->SetParent(Window);
        } else {
            Window->Remove();
            Window = NULL;
        }
    }
    if (Window && Button->OnRelease())
    {
        GetGame()->GetStateMachine()->SetState("Paused");
    }
    if (GetGame()->GetInput()->GetMouseKeyChanged(0) && GetGame()->GetInput()->GetMouseKey(0) && !GetGame()->GetInput()->GetMouseHitGUI())
    {
        std::vector<NNode*> Entities = GetGame()->GetScene()->GetNodesByType(NodeEntity);
        glm::vec3 MousePos = GetGame()->GetInput()->GetPerspMouse(.45);
        float TS = GetGame()->GetMap()->GetTileSize();
        for (unsigned int i=0;i<Entities.size();i++)
        {
            NEntity* Ent = static_cast<NEntity*>(Entities[i]);
            if (GetGame()->GetMap()->GetLevel() != GetGame()->GetMap()->GetLevel(Ent->GetRealPos()))
            {
                continue;
            }
            if (Intersects(Ent->GetPos(),glm::vec3(Ent->GetFloat("Width"),Ent->GetFloat("Height"),TS),MousePos) && glm::distance(Ent->GetPos(), Player->GetPos()) < TS*1.5f)
            {
                Ent->CallMethod("OnClick", "n", Player);
                break;
            }
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
    GetGame()->GetMap()->Init(16,16,16);
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
            default: GetGame()->GetLog()->Send("SERVER",1,"Not all events are implemented! FIXMEEE"); break;
        }
    }
    if (GetGame()->GetInput()->GetKeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
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
    GetGame()->GetMap()->Init(16,16,16);
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
                GetGame()->GetLog()->Send("SERVER",2,"Alyx: I downloaded the whole packet, and they have been chasing us ever since.");
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
    MapName = _t("default");
}
NMapState::~NMapState()
{
}
void NMapState::OnEnter()
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
    Window->SetScale(150,150);
    Window->SetPos(glm::vec3(75,75,0));
    Window->SetParent(Camera);
    OtherWindow = new NWindow("window");
    OtherWindow->SetScale(128,128);
    OtherWindow->SetParent(Camera);
    OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,64,0));
    CheckBox = new NCheckbox("checkbox");
    CheckBox->SetScale(16,16);
    CheckBox->SetPos(glm::vec3(-16,18, 0));
    CheckBox->SetParent(OtherWindow);
    CheckText = new NText("gui",GetGame()->GetLua()->Translate(UISolid));
    CheckText->SetSize(12);
    CheckText->SetPos(glm::vec3(0,20,0));
    CheckText->SetParent(OtherWindow);
    OCheckBox = new NCheckbox("checkbox");
    OCheckBox->SetScale(16,16);
    OCheckBox->SetPos(glm::vec3(-16,-7+5,0));
    OCheckBox->SetParent(OtherWindow);
    OCheckText = new NText("gui",GetGame()->GetLua()->Translate(UIOpaque));
    OCheckText->SetSize(12);
    OCheckText->SetPos(glm::vec3(0,0,0));
    OCheckText->SetParent(OtherWindow);
    Increase = new NButton("button");
    Increase->SetScale(16,16);
    Increase->SetText(_t(">"));
    Increase->SetPos(glm::vec3(64-16,37+5,0));
    Increase->SetParent(OtherWindow);
    Decrease = new NButton("button");
    Decrease->SetScale(16,16);
    Decrease->SetText(_t("<"));
    Decrease->SetPos(glm::vec3(-64+16,37+5,0));
    Decrease->SetParent(OtherWindow);
    ChangingText = new NText("gui",GetGame()->GetLua()->Translate(UITile)+_t(": 0"));
    ChangingText->SetSize(16);
    ChangingText->SetMode(1);
    ChangingText->SetPos(glm::vec3(0,43,0));
    ChangingText->SetParent(OtherWindow);
    Text = new NText("gui",GetGame()->GetLua()->Translate(UIMapEditIntro));
    Text->SetSize(15);
    Text->SetBorder(150,150);
    Text->SetPos(glm::vec3(-75,75-18,0));
    Text->SetParent(Window);
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
    glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
    Bg->SetPos(glm::vec3(Temp.x,Temp.y,0));

    SlopeUp = new NButton("button");
    SlopeUp->SetScale(16,16);
    SlopeUp->SetText(_t("^"));
    SlopeUp->SetPos(glm::vec3(0,-18,0));
    SlopeUp->SetToggleable(true);
    SlopeUp->SetParent(OtherWindow);

    SlopeDown = new NButton("button");
    SlopeDown->SetScale(16,16);
    SlopeDown->SetText(_t("v"));
    SlopeDown->SetPos(glm::vec3(0,-50,0));
    SlopeDown->SetToggleable(true);
    SlopeDown->SetParent(OtherWindow);

    SlopeRight = new NButton("button");
    SlopeRight->SetScale(16,16);
    SlopeRight->SetText(_t(">"));
    SlopeRight->SetPos(glm::vec3(16,-34,0));
    SlopeRight->SetToggleable(true);
    SlopeRight->SetParent(OtherWindow);

    SlopeLeft = new NButton("button");
    SlopeLeft->SetScale(16,16);
    SlopeLeft->SetText(_t("<"));
    SlopeLeft->SetPos(glm::vec3(-16,-34,0));
    SlopeLeft->SetToggleable(true);
    SlopeLeft->SetParent(OtherWindow);

    SlopeOff = new NButton("button");
    SlopeOff->SetScale(16,16);
    SlopeOff->SetText(_t("o"));
    SlopeOff->SetPos(glm::vec3(0,-34,0));
    SlopeOff->SetToggleable(true);
    SlopeOff->SetParent(OtherWindow);
    SaveWindow = NULL;
}
void NMapState::OnExit()
{
    GetGame()->GetScene()->RemoveByType(NodeStar);
    GetGame()->GetMap()->DeInit();
    HWindow->Remove();
    Window->Remove();
    OtherWindow->Remove();
    Bg->Remove();
    if (SaveWindow)
    {
        SaveWindow->Remove();
    }
    for (unsigned int i=0;i<EntityHighlight.size();i++)
    {
        EntityHighlight[i]->Remove();
    }
    Entities.clear();
}
void NMapState::Tick(double DT)
{
    std::vector<NNode*> Nodes = GetGame()->GetScene()->GetNodesByType(NodeEntity);
    if (Nodes.size() != EntityHighlight.size())
    {
        for (unsigned int i=0;i<EntityHighlight.size();i++)
        {
            EntityHighlight[i]->Remove();
            EntityHighlight[i] = NULL;
        }
        EntityHighlight.resize(Nodes.size(),NULL);
        EntityText.resize(Nodes.size(),NULL);
        for (unsigned int i=0;i<Nodes.size();i++)
        {
            NEntity* Ent = (NEntity*)Nodes[i];
            if (EntityHighlight[i] == NULL)
            {
                EntityHighlight[i] = new NWindow("highlight");
                EntityHighlight[i]->SetScale(glm::vec3(80,80,1));
                EntityHighlight[i]->SetColor(glm::vec4(0,1,0,1));
                EntityHighlight[i]->SetLayer(1);
                EntityHighlight[i]->SwapView();
                EntityText[i] = new NText("gui",ToMBS(Ent->GetName()));
                EntityText[i]->SetParent(EntityHighlight[i]);
                EntityText[i]->SetSize(13);
                EntityText[i]->SwapView();
                EntityText[i]->SetLayer(1);
                EntityText[i]->SetColor(glm::vec4(0,1,0,1));
                EntityText[i]->SetMode(1);
            }
            EntityText[i]->SetText(ToMBS(Ent->GetName()));
            EntityHighlight[i]->SetPos(Ent->GetPos());
        }
    }
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
    glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
    Bg->SetPos(glm::vec3(Temp.x,Temp.y,0));
    std::vector<NNode*>* Stars = GetGame()->GetScene()->GetLayer(0);
    while(Stars->size()<(W+H)/20)
    {
        new NStar();
    }
    OtherWindow->SetPos(glm::vec3(GetGame()->GetWindowWidth()-64,64,0));
    if (Increase->OnRelease() || (GetGame()->GetInput()->GetKeyChanged('E') && GetGame()->GetInput()->GetKey('E')))
    {
        CurrentTile++;
        if (CurrentTile>int(GetGame()->GetMap()->GetTileCount()))
        {
            CurrentTile = -Entities.size();
        }
        if (CurrentTile >= 0)
        {
            std::wstringstream Stream;
            Stream << GetGame()->GetLua()->Translate(UITile) << ": ";
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
    if (Decrease->OnRelease() || (GetGame()->GetInput()->GetKeyChanged('Q') && GetGame()->GetInput()->GetKey('Q')))
    {
        CurrentTile--;
        if (CurrentTile<-int(Entities.size()))
        {
            CurrentTile = GetGame()->GetMap()->GetTileCount();
        }
        if (CurrentTile >= 0)
        {
            std::wstringstream Stream;
            Stream << GetGame()->GetLua()->Translate(UITile) << ": ";
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
    if (GetGame()->GetInput()->GetKeyChanged(GLFW_KEY_ESC) && GetGame()->GetInput()->GetKey(GLFW_KEY_ESC))
    {
        if (SaveWindow)
        {
            SaveWindow->Remove();
            SaveWindow = NULL;
        } else {
            SaveWindow = new NWindow("window");
            SaveWindow->SetScale(256,256);
            glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
            SaveWindow->SetPos(glm::vec3(Temp.x,Temp.y,0));
            SaveWindow->SetParent(GetGame()->GetRender()->GetCamera());
            Input = new NTextInput("textbox");
            Input->SetScale(200,24);
            Input->SetPos(glm::vec3(0,100,0));
            Input->GetText()->SetMultiline(false);
            Input->GetText()->SetSize(15);
            Input->SetText(MapName);
            Input->SetParent(SaveWindow);
            SaveButton = new NButton("button");
            SaveButton->SetScale(64,32);
            SaveButton->SetText(GetGame()->GetLua()->Translate(UISave));
            SaveButton->SetPos(glm::vec3(0,62,0));
            SaveButton->SetShrinkWrap(true);
            SaveButton->SetParent(SaveWindow);
            LoadButton = new NButton("button");
            LoadButton->SetScale(64,32);
            LoadButton->SetText(GetGame()->GetLua()->Translate(UILoad));
            LoadButton->SetPos(glm::vec3(0,30,0));
            LoadButton->SetShrinkWrap(true);
            LoadButton->SetParent(SaveWindow);
            QuitButton = new NButton("button");
            QuitButton->SetScale(64,32);
            QuitButton->SetShrinkWrap(true);
            QuitButton->SetText(GetGame()->GetLua()->Translate(UIQuit));
            QuitButton->SetPos(glm::vec3(0,-2,0));
            QuitButton->SetParent(SaveWindow);
            MapIncrease[0] = new NButton("button");
            MapIncrease[0]->SetScale(16,16);
            MapIncrease[0]->SetPos(glm::vec3(64,-64,0));
            MapIncrease[0]->SetText(_t(">"));
            MapIncrease[0]->SetParent(SaveWindow);
            MapIncrease[1] = new NButton("button");
            MapIncrease[1]->SetScale(16,16);
            MapIncrease[1]->SetPos(glm::vec3(64,-82,0));
            MapIncrease[1]->SetText(_t(">"));
            MapIncrease[1]->SetParent(SaveWindow);
            MapIncrease[2] = new NButton("button");
            MapIncrease[2]->SetScale(16,16);
            MapIncrease[2]->SetPos(glm::vec3(64,-100,0));
            MapIncrease[2]->SetText(_t(">"));
            MapIncrease[2]->SetParent(SaveWindow);

            MapDecrease[0] = new NButton("button");
            MapDecrease[0]->SetScale(16,16);
            MapDecrease[0]->SetPos(glm::vec3(-64,-64,0));
            MapDecrease[0]->SetText(_t("<"));
            MapDecrease[0]->SetParent(SaveWindow);
            MapDecrease[1] = new NButton("button");
            MapDecrease[1]->SetScale(16,16);
            MapDecrease[1]->SetPos(glm::vec3(-64,-82,0));
            MapDecrease[1]->SetText(_t("<"));
            MapDecrease[1]->SetParent(SaveWindow);
            MapDecrease[2] = new NButton("button");
            MapDecrease[2]->SetScale(16,16);
            MapDecrease[2]->SetPos(glm::vec3(-64,-100,0));
            MapDecrease[2]->SetText(_t("<"));
            MapDecrease[2]->SetParent(SaveWindow);

            MapDim[0] = GetGame()->GetMap()->GetWidth();
            MapDim[1] = GetGame()->GetMap()->GetHeight();
            MapDim[2] = GetGame()->GetMap()->GetDepth();
            std::wstringstream String;
            String << GetGame()->GetLua()->Translate(UIWidth) << ": " << MapDim[0];
            Dimensions[0] = new NText("gui",String.str());
            Dimensions[0]->SetPos(glm::vec3(0,-64,0));
            Dimensions[0]->SetSize(16);
            Dimensions[0]->SetMode(1);
            Dimensions[0]->SetParent(SaveWindow);
            std::wstringstream Stringh;
            Stringh << GetGame()->GetLua()->Translate(UIHeight) << ": " << MapDim[1];
            Dimensions[1] = new NText("gui",Stringh.str());
            Dimensions[1]->SetPos(glm::vec3(0,-82,0));
            Dimensions[1]->SetSize(16);
            Dimensions[1]->SetMode(1);
            Dimensions[1]->SetParent(SaveWindow);
            std::wstringstream Stringd;
            Stringd << GetGame()->GetLua()->Translate(UIDepth) << ": " << MapDim[2];
            Dimensions[2] = new NText("gui",Stringd.str());
            Dimensions[2]->SetPos(glm::vec3(0,-100,0));
            Dimensions[2]->SetSize(16);
            Dimensions[2]->SetMode(1);
            Dimensions[2]->SetParent(SaveWindow);
            Apply = new NButton("button");
            Apply->SetText(GetGame()->GetLua()->Translate(UIApply));
            Apply->GetText()->SetSize(14);
            Apply->SetScale(64,16);
            Apply->SetPos(glm::vec3(0,-120,0));
            Apply->SetShrinkWrap(true);
            Apply->SetParent(SaveWindow);
            NText* TempText = new NText("gui",GetGame()->GetLua()->Translate(UIMapDim));
            TempText->SetMode(1);
            TempText->SetPos(glm::vec3(0,-34,0));
            TempText->SetSize(16);
            TempText->SetParent(SaveWindow);
        }
    }
    if (SaveWindow)
    {
        glm::vec2 Temp = GetGame()->GetWindowSize()/2.f;
        SaveWindow->SetPos(glm::vec3(Temp.x,Temp.y,0));
        if (SaveButton->OnRelease())
        {
            MapName = Input->GetEnteredText();
            GetGame()->GetMap()->Save(ToMBS(MapName));
        }
        if (LoadButton->OnRelease())
        {
            MapName = Input->GetEnteredText();
            GetGame()->GetMap()->Load(ToMBS(MapName));
        }
        if (QuitButton->OnRelease())
        {
            GetGame()->GetStateMachine()->SetState("Paused");
            return;
        }
        if (Apply->OnRelease())
        {
            MapName = Input->GetEnteredText();
            GetGame()->GetMap()->Resize(MapDim[0],MapDim[1],MapDim[2]);
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
                    case 0: Stream << GetGame()->GetLua()->Translate(UIWidth) << ": "; break;
                    case 1: Stream << GetGame()->GetLua()->Translate(UIHeight) << ": "; break;
                    case 2: Stream << GetGame()->GetLua()->Translate(UIDepth) << ": "; break;
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
                    case 0: Stream << GetGame()->GetLua()->Translate(UIWidth) << ": "; break;
                    case 1: Stream << GetGame()->GetLua()->Translate(UIHeight) << ": "; break;
                    case 2: Stream << GetGame()->GetLua()->Translate(UIDepth) << ": "; break;
                }
                Stream << MapDim[i];
                Dimensions[i]->SetText(Stream.str());
            }
        }
    }
    if (GetGame()->GetInput()->GetKeyChanged('F') && GetGame()->GetInput()->GetKey('F'))
    {
        GetGame()->GetScene()->ToggleFullBright();
    }
    if (!GetGame()->GetScene()->GetFullBright())
    {
        HWindow->SetColor(glm::vec4(0,0,0,0));
        for (unsigned int i=0;i<EntityHighlight.size();i++)
        {
            EntityHighlight[i]->SetColor(glm::vec4(0,0,0,0));
            EntityText[i]->SetColor(glm::vec4(0,0,0,0));
        }
    } else {
        HWindow->SetColor(glm::vec4(1,1,1,1));
        for (unsigned int i=0;i<EntityHighlight.size();i++)
        {
            EntityHighlight[i]->SetColor(glm::vec4(0,1,0,1));
            EntityText[i]->SetColor(glm::vec4(0,1,0,1));
        }
    }
    NCamera* Camera = GetGame()->GetRender()->GetCamera();
    float Multi = DT*5;
    if (Multi <= 1)
    {
        Camera->SetPos(Camera->GetPos()+((WantedPosition-Camera->GetPos()))*float(DT*5));
    } else {
        Camera->SetPos(WantedPosition);
    }
    if (GetGame()->GetInput()->GetKeyChanged(GLFW_KEY_UP) && GetGame()->GetInput()->GetKey(GLFW_KEY_UP))
    {
        if (GetGame()->GetMap()->GetLevel(WantedPosition-glm::vec3(0,0,500)) < int(GetGame()->GetMap()->GetDepth())-1)
        {
            WantedPosition += glm::vec3(0,0,GetGame()->GetMap()->GetTileSize());
        }
    }
    if (GetGame()->GetInput()->GetKeyChanged(GLFW_KEY_DOWN) && GetGame()->GetInput()->GetKey(GLFW_KEY_DOWN))
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
            if (GetGame()->GetInput()->GetMouseKeyChanged(0))
            {
                new NEntity(Entities[-CurrentTile-1],GetGame()->GetInput()->GetPerspMouse(.45));
            }
        }
    }
    if (GetGame()->GetInput()->GetMouseKeyChanged(1) && GetGame()->GetInput()->GetMouseKey(1) && !GetGame()->GetInput()->GetMouseHitGUI())
    {
        std::vector<NNode*> Entities = GetGame()->GetScene()->GetNodesByType(NodeEntity);
        glm::vec3 MousePos = GetGame()->GetInput()->GetPerspMouse(.45);
        for (unsigned int i=0;i<Entities.size();i++)
        {
            if (Intersects(Entities[i]->GetPos(),glm::vec3(96,96,96),MousePos))
            {
                Entities[i]->Remove();
                break;
            }
        }
    }
}
std::string NMapState::GetName()
{
    return "Mapping";
}
