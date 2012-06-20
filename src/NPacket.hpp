#ifndef NAELSTROF_PACKET
#define NAELSTROF_PACKET

class NPacketIntroduction
{
public:
	NPacketIntroduction(std::string i_Name);
	NPacketIntroduction();
	char Name[64];
};

class NPacketResponse
{
public:
	NPacketResponse(char i_Response);
	NPacketResponse();
	char Response;
};

class NPacketNewPlayer
{
public:
	NPacketNewPlayer(NPlayer* Player);
	NPacketNewPlayer();
	char Name[64];
	unsigned int ID;
	glm::vec3 Pos;
};

class NPacketPlayer
{
public:
	NPacketPlayer(NPlayer* Player);
	NPacketPlayer();
	unsigned int ID;
	float Direction;
	bool Moving;
	glm::vec3 Pos;
	glm::vec3 Vel;
};

class NPacketDisconnectPlayer
{
public:
	NPacketDisconnectPlayer(unsigned int i_ID);
	NPacketDisconnectPlayer();
	unsigned int ID;
};

class NPacketChat
{
public:
	NPacketChat(std::string Text);
	NPacketChat();
	unsigned int ID;
	char Message[128];
};

class NPacket
{
public:
	NPacket();
	NPacket(std::string Data, ENetEvent i_Event);
	NPacket(char Response, ENetEvent i_Event);
	NPacket(std::string Name);
	NPacket(NPlayer* Player);
	void Broadcast();
	void Send(ENetPeer* Peer);
	void Merge();
private:
	ENetEvent Event;
	unsigned int NewPlayerCount;
	std::vector<NPacketNewPlayer> NewPlayers;
	unsigned int PlayerCount;
	std::vector<NPacketPlayer> Players;
	unsigned int DisconnectPlayerCount;
	std::vector<NPacketDisconnectPlayer> DisconnectPlayers;
	unsigned int ChatCount;
	std::vector<NPacketChat> Chat;
	unsigned int IntroductionCount;
	std::vector<NPacketIntroduction> Introductions;
	unsigned int ResponseCount;
	std::vector<NPacketResponse> Responses;
};

class NPacketHandler
{
public:
	NPacketHandler();
	~NPacketHandler();
	void RemovePlayer(unsigned int ID);
	void AddPlayer(NPlayer* Player);
	std::vector<NPlayer*>* GetNewPlayers();
	std::vector<unsigned int>* GetDisconnectPlayers();
	std::vector<NPlayer*>* GetActivePlayers();
	NPlayer* GetPlayer();
	void SetPlayer(NPlayer* Player);
private:
	NPlayer* MainPlayer;
	std::vector<NPlayer*> ActivePlayers;
	std::vector<NPlayer*> NewPlayers;
	std::vector<unsigned int> DisconnectPlayers;
};

#endif
