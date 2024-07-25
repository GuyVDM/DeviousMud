#pragma once

#include "Shared/Game/SpriteTypes.hpp"

/// <summary>
/// Basic tile entity.
/// </summary>
class TileEntity
{
public:
	const static U32 TILE_ENTITY_VERSION = 1;

	Graphics::SpriteType SpriteType = Graphics::SpriteType::NONE;
	U32                  Frame = 0;

	virtual Ref<TileEntity> Clone();
	
	const bool operator==(const TileEntity* _other);

	const bool operator!=(const TileEntity* _other);

	template<class Archive>
	inline void serialize(Archive& ar, const std::uint32_t version)
	{
		ar(SpriteType, Frame);
	}

public:
	TileEntity() = default;
	virtual ~TileEntity() = default;
};

//Versioning for serialization
CEREAL_CLASS_VERSION(TileEntity, TileEntity::TILE_ENTITY_VERSION);