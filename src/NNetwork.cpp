#include "NEngine.hpp"

NNetwork::NNetwork()
{
	if (enet_initialize())
	{
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize enet!\n";
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
	SetColor(Blue);
	std::cout << "NETWORK INFO: ";
	ClearColor();
	std::cout << "Creating server on port " << Address.port << " with " << PlayerSlots << " player slots.\n";
	Server = enet_host_create(&Address,PlayerSlots,2,0,0);
	if (!Server)
	{
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to create server!\n";
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
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to create client!\n";
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
		SetColor(Yellow);
		std::cout << "NETWORK WARN: ";
		ClearColor();
		std::cout << "We have a server and client running on the same host! Assuming we're a server...\n";
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
			SetColor(Red);
			std::cout << "NETWORK ERROR: ";
			ClearColor();
			std::cout << "Failed to poll for events!\n";
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
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to poll for events!\n";
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
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to connect to host " << HostName << " on port " << Port << "!\n";
		return Fail;
	}
	ENetEvent Event;
	if (enet_host_service(Client, &Event, 2500) > 0 && Event.type == ENET_EVENT_TYPE_CONNECT)
	{
		SetColor(Blue);
		std::cout << "NETWORK INFO: ";
		ClearColor();
		std::cout << "Successfully connected to " << HostName << " on port " << Port << ".\n";
		return Success;
	} else {
		SetColor(Red);
		std::cout << "NETWORK ERROR: ";
		ClearColor();
		std::cout << "Failed to connect to host " << HostName << " on port " << Port << "!\n";
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
				SetColor(Blue);
				std::cout << "NETWORK INFO: ";
				ClearColor();
				std::cout << "Succesfully disconnected with " << HostName << "\n";
				Host = NULL;
				return;
			}
		}
	}
	SetColor(Yellow);
	std::cout << "NETWORK WARN: ";
	ClearColor();
	std::cout << "Server didn't respond to disconnect!\n";
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
