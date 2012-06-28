#include "NEngine.hpp"
NPacketNewPlayer::NPacketNewPlayer(NPlayer* Player)
{
    Pos = Player->GetPos();
    strcpy(Name,Player->GetName().c_str());
    ID = Player->GetID();
}
NPacketNewPlayer::NPacketNewPlayer()
{
}

NPacketPlayer::NPacketPlayer(NPlayer* Player)
{
    ID = Player->GetID();
    Pos = Player->GetPos();
    Direction = Player->GetDirection();
    Vel = Player->GetVel();
    Moving = Player->GetMoving();
}
NPacketPlayer::NPacketPlayer()
{
}

NPacketDisconnectPlayer::NPacketDisconnectPlayer(unsigned int i_ID)
{
    ID = i_ID;
}
NPacketDisconnectPlayer::NPacketDisconnectPlayer()
{
}

NPacketChat::NPacketChat(std::string Text)
{
    strcpy(Message,Text.c_str());
}
NPacketIntroduction::NPacketIntroduction(std::string i_Name)
{
    strcpy(Name,i_Name.c_str());
}
NPacketIntroduction::NPacketIntroduction()
{
}
NPacketChat::NPacketChat()
{
}
NPacketResponse::NPacketResponse(char i_Response)
{
    Response = i_Response;
}
NPacketResponse::NPacketResponse()
{
}

NPacket::NPacket(NPlayer* Player)
{
    PlayerCount = 1;
    Players.push_back(NPacketPlayer(Player));
    NewPlayerCount = 0;
    DisconnectPlayerCount = 0;
    IntroductionCount = 0;
    ResponseCount = 0;
}

NPacket::NPacket()
{
    PlayerCount = 0;
    std::vector<NNode*>* World = GetGame()->GetScene()->GetLayer(1); // Grab everything that's in the world
    for (unsigned int i=0;i<World->size();i++)
    {
        NNode* Node = (*World)[i];
        if (Node->GetType() == NodePlayer)
        {
            Players.push_back(NPacketPlayer((NPlayer*)Node));
            PlayerCount++;
        }
    }
    NewPlayerCount = 0;
    std::vector<NPlayer*>* NP = GetGame()->GetPacketHandler()->GetNewPlayers();
    for (unsigned int i=0;i<NP->size();i++)
    {
        NewPlayers.push_back(NPacketNewPlayer((*NP)[i]));
        NewPlayerCount++;
    }
    NP->clear();
    DisconnectPlayerCount = 0;
    std::vector<unsigned int>* DP = GetGame()->GetPacketHandler()->GetDisconnectPlayers();
    for (unsigned int i=0;i<DP->size();i++)
    {
        DisconnectPlayers.push_back(NPacketDisconnectPlayer((*DP)[i]));
        DisconnectPlayerCount++;
    }
    DP->clear();
    IntroductionCount = 0;
    ResponseCount = 0;
}

NPacket::NPacket(std::string Name)
{
    PlayerCount = 0;
    NewPlayerCount = 0;
    DisconnectPlayerCount = 0;
    IntroductionCount = 1;
    Introductions.push_back(NPacketIntroduction(GetGame()->GetConfig()->GetString("PlayerName")));
    ResponseCount = 0;
}

NPacket::NPacket(char Response, ENetEvent i_Event)
{
    Event = i_Event;
    std::vector<NPlayer*>* ActivePlayers = GetGame()->GetPacketHandler()->GetActivePlayers();
    PlayerCount = 0;
    NewPlayerCount = ActivePlayers->size();
    for (unsigned int i=0;i<ActivePlayers->size();i++)
    {
        NewPlayers.push_back(NPacketNewPlayer((*ActivePlayers)[i]));
    }
    DisconnectPlayerCount = 0;
    IntroductionCount = 0;
    ResponseCount = 1;
    Responses.push_back(NPacketResponse(Response));
}

void NPacket::Broadcast()
{
    //std::cout << "s:" <<  PlayerCount << " " << NewPlayerCount << " " << DisconnectPlayerCount << " " << IntroductionCount << " " << ResponseCount << "\n";
    std::string Data;
    Data.append((char*)&PlayerCount,4);
    for (unsigned int i=0;i<PlayerCount;i++)
    {
        Data.append((char*)&Players[i],sizeof(NPacketPlayer));
    }
    Data.append((char*)&NewPlayerCount,4);
    for (unsigned int i=0;i<NewPlayerCount;i++)
    {
        Data.append((char*)&NewPlayers[i],sizeof(NPacketNewPlayer));
    }
    Data.append((char*)&DisconnectPlayerCount,4);
    for (unsigned int i=0;i<DisconnectPlayerCount;i++)
    {
        Data.append((char*)&DisconnectPlayers[i],sizeof(NPacketDisconnectPlayer));
    }
    Data.append((char*)&IntroductionCount,4);
    for (unsigned int i=0;i<IntroductionCount;i++)
    {
        Data.append((char*)&Introductions[i],sizeof(NPacketIntroduction));
    }
    Data.append((char*)&ResponseCount,4);
    for (unsigned int i=0;i<ResponseCount;i++)
    {
        Data.append((char*)&Responses[i],sizeof(NPacketResponse));
    }
    GetGame()->GetNetwork()->Broadcast(Data,ENET_PACKET_FLAG_UNSEQUENCED);
}

void NPacket::Send(ENetPeer* Peer)
{
    //std::cout << "s:" <<  PlayerCount << " " << NewPlayerCount << " " << DisconnectPlayerCount << " " << IntroductionCount << " " << ResponseCount << "\n";
    std::string Data;
    Data.append((char*)&PlayerCount,4);
    for (unsigned int i=0;i<PlayerCount;i++)
    {
        Data.append((char*)&Players[i],sizeof(NPacketPlayer));
    }
    Data.append((char*)&NewPlayerCount,4);
    for (unsigned int i=0;i<NewPlayerCount;i++)
    {
        Data.append((char*)&NewPlayers[i],sizeof(NPacketNewPlayer));
    }
    Data.append((char*)&DisconnectPlayerCount,4);
    for (unsigned int i=0;i<DisconnectPlayerCount;i++)
    {
        Data.append((char*)&DisconnectPlayers[i],sizeof(NPacketDisconnectPlayer));
    }
    Data.append((char*)&IntroductionCount,4);
    for (unsigned int i=0;i<IntroductionCount;i++)
    {
        Data.append((char*)&Introductions[i],sizeof(NPacketIntroduction));
    }
    Data.append((char*)&ResponseCount,4);
    for (unsigned int i=0;i<ResponseCount;i++)
    {
        Data.append((char*)&Responses[i],sizeof(NPacketResponse));
    }
    GetGame()->GetNetwork()->Send(Peer,Data,ENET_PACKET_FLAG_UNSEQUENCED);
}

NPacket::NPacket(std::string Data, ENetEvent i_Event)
{
    Event = i_Event;
    Players.clear();
    NewPlayers.clear();
    unsigned int Pen = 0;
    Data.copy((char*)&PlayerCount,4,Pen);
    Pen += 4;
    for (unsigned int i=0;i<PlayerCount;i++)
    {
        NPacketPlayer Player;
        Data.copy((char*)&Player,sizeof(NPacketPlayer),Pen);
        Players.push_back(Player);
        Pen += sizeof(NPacketPlayer);
    }
    Data.copy((char*)&NewPlayerCount,4,Pen);
    Pen += 4;
    for (unsigned int i=0;i<NewPlayerCount;i++)
    {
        NPacketNewPlayer Player;
        Data.copy((char*)&Player,sizeof(NPacketNewPlayer),Pen);
        NewPlayers.push_back(Player);
        Pen += sizeof(NPacketNewPlayer);
    }
    Data.copy((char*)&DisconnectPlayerCount,4,Pen);
    Pen += 4;
    for (unsigned int i=0;i<DisconnectPlayerCount;i++)
    {
        NPacketDisconnectPlayer Player;
        Data.copy((char*)&Player,sizeof(NPacketDisconnectPlayer),Pen);
        DisconnectPlayers.push_back(Player);
        Pen += sizeof(NPacketDisconnectPlayer);
    }
    Data.copy((char*)&IntroductionCount,4,Pen);
    Pen += 4;
    for (unsigned int i=0;i<IntroductionCount;i++)
    {
        NPacketIntroduction Intro;
        Data.copy((char*)&Intro,sizeof(NPacketIntroduction),Pen);
        Introductions.push_back(Intro);
        Pen += sizeof(NPacketIntroduction);
    }
    Data.copy((char*)&ResponseCount,4,Pen);
    Pen += 4;
    for (unsigned int i=0;i<ResponseCount;i++)
    {
        NPacketResponse Response;
        Data.copy((char*)&Response,sizeof(NPacketIntroduction),Pen);
        Responses.push_back(Response);
        Pen += sizeof(NPacketResponse);
    }
}

void NPacket::Merge()
{
    //std::cout << "g:" <<  PlayerCount << " " << NewPlayerCount << " " << DisconnectPlayerCount << " " << IntroductionCount << " " << ResponseCount << "\n";
    for (unsigned int i=0;i<NewPlayerCount;i++)
    {
        if (GetGame()->GetScene()->GetNodeByID(NewPlayers[i].ID) != NULL)
        {
            continue;
        }
        NPlayer* Player = GetGame()->GetScene()->AddPlayer(NewPlayers[i].Name);
        Player->SetID(NewPlayers[i].ID);
        if (Player->GetName() == GetGame()->GetConfig()->GetString("PlayerName"))
        {
            Player->SetControl();
            GetGame()->GetPacketHandler()->SetPlayer(Player);
        }
        NTerminal::SetColor(Blue);
        std::cout << "PACKET INFO: ";
        NTerminal::ClearColor();
        std::cout << Player->GetName() << " has joined the game!\n";
    }
    for (unsigned int i=0;i<DisconnectPlayerCount;i++)
    {
        GetGame()->GetScene()->RemoveByID(DisconnectPlayers[i].ID);
    }
    for (unsigned int i=0;i<PlayerCount;i++)
    {
        if (!GetGame()->IsServer())
        {
            NPlayer* Player = (NPlayer*)GetGame()->GetScene()->GetNodeByID(Players[i].ID);
            if (Player)
            {
                Player->SetWantedPosition(Players[i].Pos);
                if (Player != GetGame()->GetPacketHandler()->GetPlayer())
                {
                    Player->SetVel(Players[i].Vel);
                    Player->SetDirection(Players[i].Direction);
                    Player->SetMoving(Players[i].Moving);
                }
            }
        } else {
            NPlayer* Player = (NPlayer*)GetGame()->GetScene()->GetNodeByID(*(unsigned int*)Event.peer->data);
            if (Player)
            {
                Player->SetDirection(Players[i].Direction);
                Player->SetMoving(Players[i].Moving);
            }
        }
    }
    for (unsigned int i=0;i<IntroductionCount;i++)
    {
        NTerminal::SetColor(Blue);
        std::cout << "PACKET INFO: ";
        NTerminal::ClearColor();
        std::cout << Introductions[i].Name << " has joined the game!\n";
        NPlayer* Player = GetGame()->GetScene()->AddPlayer(std::string(Introductions[i].Name));
        Player->SetID(GetGame()->GetPacketHandler()->GetActivePlayers()->size()+1);
        Player->SetPos(512,512,1024);
        GetGame()->GetPacketHandler()->AddPlayer(Player);
        NPacket Packet('0',Event);
        Packet.Send(Event.peer);
        *(unsigned int*)Event.peer->data = Player->GetID();
    }
    for (unsigned int i=0;i<ResponseCount;i++)
    {
        switch(Responses[i].Response)
        {
            case '0':
            {
                NTerminal::SetColor(Blue);
                std::cout << "SERVER INFO: ";
                NTerminal::ClearColor();
                std::cout << "Server accepted our introduction!\n";
                break;
            }
            default:
            {
                NTerminal::SetColor(Red);
                std::cout << "SERVER ERROR: ";
                NTerminal::ClearColor();
                std::cout << "Recieved an unkown response from the server! Packet could be garbled or someone's out of date!\n";
                break;
            }
        }
    }
}

NPacketHandler::NPacketHandler()
{
    MainPlayer = NULL;
}
NPacketHandler::~NPacketHandler()
{
}
void NPacketHandler::AddPlayer(NPlayer* Player)
{
    ActivePlayers.push_back(Player);
    NewPlayers.push_back(Player);
}
std::vector<NPlayer*>* NPacketHandler::GetNewPlayers()
{
    return &NewPlayers;
}
std::vector<unsigned int>* NPacketHandler::GetDisconnectPlayers()
{
    return &DisconnectPlayers;
}
std::vector<NPlayer*>* NPacketHandler::GetActivePlayers()
{
    return &ActivePlayers;
}
void NPacketHandler::SetPlayer(NPlayer* Player)
{
    MainPlayer = Player;
}
NPlayer* NPacketHandler::GetPlayer()
{
    return MainPlayer;
}
void NPacketHandler::RemovePlayer(unsigned int ID)
{
    for (unsigned int i=0;i<ActivePlayers.size();i++)
    {
        if (ActivePlayers[i]->GetID() == ID)
        {
            DisconnectPlayers.push_back(ID);
            ActivePlayers.erase(ActivePlayers.begin()+i);
            return;
        }
    }
}
