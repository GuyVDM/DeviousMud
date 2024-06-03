#pragma once

#include "Core/Game/Entity/Definition/EntityDef.h"

struct ClientInfo;
using RefClientInfo = std::shared_ptr<ClientInfo>;

class CombatHandler 
{

public:
	/// <summary>
	/// 
	/// </summary>
	const static bool engage(std::shared_ptr<Player> _a, std::shared_ptr<Entity> _b);

	const static bool engage(std::shared_ptr<NPC> _a, std::shared_ptr<Entity> _b);

	static void hit(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b);

	static bool is_adjacent(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b);
public:
	CombatHandler() = default;
	~CombatHandler() = default;

private:
	static void queue_combat_packet(RefClientInfo _client, DM::Utils::UUID _targetUUID);

	static bool in_range(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b, int32_t& attackRange);


};