#pragma once
#include "Shared/Game/PlayerDetails.h"

#include "Shared/Utilities/EventListener.h"

#include "Shared/Utilities/UUID.hpp"

#include "Core/Rendering/Sprite/Sprite.h"

#include <unordered_map>

#pragma region FORWARD_DECLERATIONS
namespace Graphics 
{
	class Renderer;
}

template <typename Parameter>
class EventListener;
#pragma endregion

class PlayerHandler 
{
public:
	EventListener<uint64_t> on_local_player_assigned;

	EventListener<uint64_t> on_player_created;

	EventListener<uint64_t> on_player_removed;

	PlayerDetails& get_details(const uint64_t& _playerhandle);

	PlayerDetails& get_local_player_details();

	void create_player(const uint64_t& _playerhandle, PlayerDetails& _details);

	void remove_player(const uint64_t& _playerhandle);

public:
	PlayerHandler();
	~PlayerHandler();

private:
	PlayerHandler(const PlayerHandler&) = delete;

private:
	void register_local_player(uint64_t _localPlayerId);

private:
	std::unordered_map<DEVIOUSMUD::RANDOM::UUID, PlayerDetails> players;
	uint64_t localPlayerId;
};