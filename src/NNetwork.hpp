/**
 * @file NNetwork.hpp
 * @brief Handles sending data through the network.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-06-20
 */
#ifndef NAELSTROF_NETWORK
#define NAELSTROF_NETWORK

/**
 * @brief Exposes netowrking interfaces through the network.
 */
class NNetwork
{
public:
	NNetwork();
	~NNetwork();
	/**
	 * @brief Grabs an Enet event if it exists and stores it in Event.
	 *
	 * @param Event A pointer to store the Enet event in.
	 *
	 * @return True if there is an event, or false if there was no events to poll.
	 */
	bool PollEvent(ENetEvent* Event);
	/**
	 * @brief Creates a server to interface with.
	 *
	 * @return True on success, false on failure.
	 */
	bool CreateServer();
	/**
	 * @brief Creates a client to interface with, must call Connect(std::string,unsigned int) in order for it to work properly.
	 *
	 * @return True on success, false on failure.
	 */
	bool CreateClient();
	/**
	 * @brief Connects to the specified hostname and port.
	 *
	 * @param i_HostName The hostname to the server we're connecting to, can be an ip or whatever.
	 * @param Port The port the server is serving from.
	 *
	 * @return True if we succeeded, false if we failed.
	 */
	bool Connect(std::string i_HostName, unsigned int Port);
	/**
	 * @brief If we have previously Connect()'ed, it politely attempts to disconnect itself. If it fails after a second it forcibly disconnects.
	 */
	void Disconnect();
	/**
	 * @brief Broadcasts a string of data with the given flags to all connected peers.
	 *
	 * @param Data The data we want to send.
	 * @param Flags The ENet flags we want to apply to the packet.
	 */
	void Broadcast(std::string Data, ENetPacketFlag Flags);
	/**
	 * @brief Sends a string of data to the given peer.
	 *
	 * @param Peer The peer we want to receieve the packet.
	 * @param Data The data we want them to recevieve.
	 * @param Flags The flags we want applied to the packet.
	 */
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
