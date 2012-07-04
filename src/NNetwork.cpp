#include "NEngine.hpp"

NNetwork::NNetwork()
{
    if (enet_initialize())
    {
        GetGame()->GetLog()->Send("NETWORK",0,"Failed to initialize enet!");
    }
    Server = NULL;
    PlayerSlots = 0;
    Client = NULL;
    Host = NULL;
}

bool NNetwork::CreateServer()
{
    if (Server)
    {
        enet_host_destroy(Server);
    }
    Address.host = ENET_HOST_ANY;
    Address.port = GetGame()->GetConfig()->GetFloat("Port");
    PlayerSlots = GetGame()->GetConfig()->GetFloat("PlayerSlots");
    std::stringstream Message;
    Message << "Creating server on port " << Address.port << " with " << PlayerSlots << " player slots.\n";
    GetGame()->GetLog()->Send("NETWORK",2,Message.str());
    Server = enet_host_create(&Address,PlayerSlots,2,0,0);
    if (!Server)
    {
        GetGame()->GetLog()->Send("NETWORK",0,"Failed to create server!");
        return Fail;
    }
    return Success;
}

bool NNetwork::CreateClient()
{
    if (Client)
    {
        enet_host_destroy(Client);
    }
    Client = enet_host_create(NULL,1,2,0,0);
    if (!Client)
    {
        GetGame()->GetLog()->Send("NETWORK",0,"Failed to create client!");
        return Fail;
    }
    return Success;
}

bool NNetwork::PollEvent(ENetEvent* Event)
{
    if (!Server && !Client)
    {
        return false;
    }
    if (Server && Client)
    {
        GetGame()->GetLog()->Send("NETWORK",1,"We have a server and client running on the same host! Assuming we're a server...");
        enet_host_destroy(Client);
        Client = NULL;
    }

    if (Server)
    {
        int Check = enet_host_service(Server,Event,0);
        if (Check > 0)
        {
            return true;
        }
        if (Check < 0)
        {
            GetGame()->GetLog()->Send("NETWORK",0,"Failed to poll for events!");
        }
        return false;
    }
    int Check = enet_host_service(Client,Event,0);
    if (Check > 0)
    {
        return true;
    }
    if (Check < 0)
    {
        GetGame()->GetLog()->Send("NETWORK",0,"Failed to poll for events!");
    }
    return false;
}


NNetwork::~NNetwork()
{
    if (Client)
    {
        enet_host_destroy(Client);
    }
    if (Server)
    {
        enet_host_destroy(Server);
    }
    enet_deinitialize();
}

bool NNetwork::Connect(std::string i_HostName, unsigned int Port)
{
    if (Host)
    {
        Disconnect();
    }
    HostName = i_HostName;
    Address.port = Port;
    enet_address_set_host(&Address, HostName.c_str());

    Host = enet_host_connect(Client,&Address,2,0);
    if (!Host)
    {
        std::stringstream Message;
        Message << "Failed to connect to host " << HostName << " on port " << Port << "!";
        GetGame()->GetLog()->Send("NETWORK",0,Message.str());
        return Fail;
    }
    ENetEvent Event;
    if (enet_host_service(Client, &Event, 2500) > 0 && Event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::stringstream Message;
        Message << "Successfully connected to " << HostName << " on port " << Port << ".";
        GetGame()->GetLog()->Send("NETWORK",2,Message.str());
        return Success;
    } else {
        std::stringstream Message;
        Message << "Failed to connect to host " << HostName << " on port " << Port << "!";
        GetGame()->GetLog()->Send("NETWORK",0,Message.str());
        Host = NULL;
        return Fail;
    }
}

void NNetwork::Disconnect()
{
    if (!Host)
    {
        return;
    }
    enet_peer_disconnect(Host,0);
    ENetEvent Event;
    while (enet_host_service(Client,&Event,1000) > 0)
    {
        switch (Event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                enet_packet_destroy(Event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                GetGame()->GetLog()->Send("NETWORK",2,std::string("Successfully disconnected with ") + HostName + ".");
                Host = NULL;
                return;
            }
        }
    }
    GetGame()->GetLog()->Send("NETWORK",1,"Server didn't respond to disconnect!");
    enet_peer_reset(Host);
    Host = NULL;
}

void NNetwork::Broadcast(std::string Data, ENetPacketFlag Flags)
{
    ENetPacket* Packet = enet_packet_create(Data.c_str(),Data.length(),Flags);
    if (Server)
    {
        enet_host_broadcast(Server,0,Packet);
    }
    if (Client)
    {
        enet_host_broadcast(Client,0,Packet);
    }
}

void NNetwork::Send(ENetPeer* Peer, std::string Data, ENetPacketFlag Flags)
{
    ENetPacket* Packet = enet_packet_create(Data.c_str(),Data.length(),Flags);
    enet_peer_send(Peer,0,Packet);
}
