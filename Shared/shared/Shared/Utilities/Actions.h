#pragma once
#include "UUID.hpp"
#include <cstdint>

namespace DM
{
	namespace Actions
	{
		enum class e_SubjectType : uint8_t
		{
			PLAYER = 0,
			NPC = 1,
			OBJECT = 2
		};

		enum class e_ActionType
		{
			ATTACK = 0,
			TALK = 1
		};

		struct Action
		{
			DM::Utils::UUID fromEntityId;
			DM::Utils::UUID toTargetId;
			e_SubjectType   subjectType;
			e_ActionType    actionType;
		};
	}
}
