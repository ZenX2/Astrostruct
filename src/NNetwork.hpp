#ifndef NAELSTROF_NETWORK
#define NAELSTROF_NETWORK

class NNetwork
{
public:
	NNetwork();
	~NNetwork();
	bool PollEvent(ENetEvent* Event);
	bool CreateServer();
	bool CreateClient();
	bool Connect(std::string i_HostName, unsigned int Port);
	void Disconnect();
	void Broadcast(std::string Data, ENetPacketFlag Flags);
	void Send(ENetPeer* Peer, std::string Data, ENetPacketFlag Flags);
private:
	ENetAddress Address;
	ENetHost* Server;
	unsigned int PlayerSlots;

	std::string HostName;
	ENetHost* Client;
	ENetPeer* Host;
};

#endif
