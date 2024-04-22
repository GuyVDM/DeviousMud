#pragma once
#include <unordered_map>
#include <memory>

#pragma region FORWARD DECLERATIONS

typedef struct _ENetEvent ENetEvent;

typedef struct ClientInfo ClientInfo;

typedef struct _ENetHost ENetHost;

namespace Packets 
{
	struct s_PacketHeader;
}

#pragma endregion

using RefClientInfo = std::shared_ptr<ClientInfo>;

namespace Server
{
	class EventHandler
	{
	public:
		template<typename T>
		constexpr static std::unique_ptr<T> transform_packet(std::unique_ptr<Packets::s_PacketHeader> _packet);

		static void queue_incoming_event(ENetEvent* _event, RefClientInfo& _clientinfo);

		static void handle_queud_events(ENetHost* m_host);

	private:
		static void handle_client_specific_packets(RefClientInfo& _client, ENetHost* _host);
	};

	template<typename T>
	inline constexpr std::unique_ptr<T> EventHandler::transform_packet(std::unique_ptr<Packets::s_PacketHeader> _packet)
	{
		return std::unique_ptr<T>(static_cast<T*>(_packet.release()));
	}
}