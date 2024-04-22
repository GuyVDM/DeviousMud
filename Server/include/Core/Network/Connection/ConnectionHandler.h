#pragma once
#include <unordered_map>

#include <memory>

#include <optional>

#pragma region FORWARD_DECLERATIONS

typedef struct _ENetPeer ENetPeer;
typedef struct ClientInfo ClientInfo;
typedef unsigned int enet_uint32;

#pragma endregion

using RefClientInfo = std::shared_ptr<ClientInfo>;

namespace Server 
{
	class ConnectionHandler
	{
	public:
		/// <summary>
		/// Registers the client to the server.
		/// </summary>
		/// <param name="_clienthandle"></param>
		/// <param name="_peer"></param>
		void register_client(ENetPeer* _peer);

		/// <summary>
		/// Removes any data associated regarding a specific client.
		/// </summary>
		/// <param name=""></param>
		void disconnect_client(const enet_uint32& _clienthandle);

		/// <summary>
		/// Try returning a existing client peer based on the index.
		/// </summary>
		/// <returns></returns>
		RefClientInfo get_client_info(const enet_uint32& _clienthandle);;

		/// <summary>
		/// Get a read only vector with all the current registered client handles.
		/// </summary>
		/// <returns></returns>
		const std::vector<enet_uint32>& get_client_handles() const;

		/// <summary>
		/// The amount of ticks before the client gets pinged a warning.
		/// </summary>
		const uint32_t TICKS_TILL_TIMEOUT_WARNING = 18000; // 3 hours

		/// <summary>
		/// The amount of time a client is allowed to idle for before it gets disconnected.
		/// </summary>
		const uint32_t TICKS_TILL_TIMEOUT = 36000; // 6 hours

		/// <summary>
		/// The amount of tick cycles needed for the client to be inactive before the server will send out a ping to a client to see if it's still there.
		/// </summary>
		const uint32_t PING_TICK_INTERVAL = 10; // 6 seconds.

		/// <summary>
		/// Amount of ticks of no response before the client gets disconnected serverside.
		/// </summary>
		const uint32_t MAX_TICK_INTERVAL_NO_RESPONSE = 40; // 24 seconds

	private:
		/// <summary>
		/// Increments all clients their idle timers.
		/// </summary>
		void update_idle_timers();


	public:
		ConnectionHandler() = default;
		~ConnectionHandler() = default;

	private:
		ConnectionHandler(ConnectionHandler&) = delete;

	private:
		std::vector<enet_uint32>                       m_clientHandles;
		std::unordered_map<enet_uint32, RefClientInfo> m_clientInfo;

		friend class NetworkHandler;
	};
}