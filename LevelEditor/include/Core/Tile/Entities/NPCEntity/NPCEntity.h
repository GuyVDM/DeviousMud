#pragma once
#include "Core/Tile/Entities/TileEntity/TileEntity.h"

/// <summary>
/// NPC entity, besides visuals it holds a NPC indentifier and how long until it respawns.
/// </summary>
class NPCEntity : public TileEntity
{
public:
	const static U32 NPC_ENTITY_VERSION = 1;

	U32   NpcId = 0;
	float RespawnTime = 1.0f;

	inline virtual Ref<TileEntity> Clone() override;

	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t version)
	{
		ar(cereal::base_class<TileEntity>(this));

		switch (version)
		{
			case 1:
			{
				ar(NpcId, RespawnTime);
			}
			break;
		}
	}
};

CEREAL_REGISTER_TYPE(NPCEntity);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TileEntity, NPCEntity)

//Versioning for serialization
CEREAL_CLASS_VERSION(NPCEntity, NPCEntity::NPC_ENTITY_VERSION);