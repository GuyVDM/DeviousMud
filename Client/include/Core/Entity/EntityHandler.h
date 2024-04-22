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
	Utilities::vec2 m_startPos;
	Utilities::vec2 m_currentPos;
	Utilities::vec2 m_endPos;

	std::vector<Utilities::ivec2> m_path;

	bool  m_bIsDirty = false;
	float m_elapsedTime = 0.0f;

	friend struct _PlayerData;
};

/// <summary>
/// Holds data to simulating the player movement, alongside entity stats.
/// </summary>
typedef struct _PlayerData 
{ 
	PlayerDetails    m_details;
	SimPosition      m_simPos;
	Graphics::Sprite m_sprite;

	void set_position_from_server(const Utilities::ivec2 _position);

}   PlayerData;

struct EntityData 
{
};

/// <summary>
/// Tracks everything that's related to the players that are registered.
/// </summary>
class EntityHandler 
{
public:
	EventListener<uint64_t> on_local_player_assigned;

	EventListener<uint64_t> on_player_created;

	EventListener<uint64_t> on_player_removed;

	PlayerData& get_data(const uint64_t _playerhandle);

	PlayerData& get_local_player_data();

	void create_player(const uint64_t _playerhandle, PlayerDetails& _details);

	void remove_player(const uint64_t _playerhandle);

public:
	EntityHandler();
	~EntityHandler();

private:
	EntityHandler(const EntityHandler&) = delete;

private:
	void register_local_player(uint64_t _localPlayerId);

private:
	std::unordered_map<DM::Utils::UUID, PlayerData> m_players;
	uint64_t m_localPlayerId;
};