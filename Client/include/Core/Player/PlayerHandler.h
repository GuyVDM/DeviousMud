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

/// <summary>
/// Simulates the path that is taken by an actor based on server input.
/// </summary>
class SimPosition 
{
public:
	EventListener<void> on_reached_dest;

	void update();

	const bool& is_dirty() const;

	const Utilities::vec2 get_position() const;

public:
	~SimPosition() = default;

	explicit SimPosition();
	explicit SimPosition(const Utilities::_vec2& _startPos);

private:
	void set_target(const Utilities::vec2& _details);

private:
	Utilities::vec2 startPos;
	Utilities::vec2 currentPos;
	Utilities::vec2 endPos;

	std::vector<Utilities::ivec2> path;

	bool  bIsDirty = false;
	float elapsedTime = 0.0f;

	friend struct _PlayerData;
};

/// <summary>
/// Holds data to simulating the player movement, alongside entity stats.
/// </summary>
typedef struct _PlayerData 
{ 
	PlayerDetails    details;
	SimPosition      simPos;
	Graphics::Sprite sprite;

	void set_position_from_server(const Utilities::ivec2 _position);

}   PlayerData;

/// <summary>
/// Tracks everything that's related to the players that are registered.
/// </summary>
class PlayerHandler 
{
public:
	EventListener<uint64_t> on_local_player_assigned;

	EventListener<uint64_t> on_player_created;

	EventListener<uint64_t> on_player_removed;

	PlayerData& get_data(const uint64_t& _playerhandle);

	PlayerData& get_local_player_data();

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
	std::unordered_map<DEVIOUSMUD::RANDOM::UUID, PlayerData> players;
	uint64_t localPlayerId;
};