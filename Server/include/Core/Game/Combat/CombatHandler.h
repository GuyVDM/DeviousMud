#pragma once

#include "Core/Game/Entity/Definition/EntityDef.h"

class CombatHandler 
{
public:
	/// <summary>
	/// 
	/// </summary>
	const static bool engage(std::shared_ptr<Player> _a, std::shared_ptr<Entity> _b);

	const static bool engage(std::shared_ptr<NPC> _a, std::shared_ptr<Entity> _b);

	static void hit(std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b);
public:
	CombatHandler() = default;
	~CombatHandler() = default;

private:
};