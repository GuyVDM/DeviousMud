#include "precomp.h"

#include "Core/Tile/Entities/TileEntity/TileEntity.h"

Ref<TileEntity> TileEntity::Clone()
{
	return std::make_shared<TileEntity>(*this);
}

const bool TileEntity::operator==(const TileEntity* _other)
{
	return _other->Frame == Frame && _other->SpriteType == SpriteType;
}

const bool TileEntity::operator!=(const TileEntity* _other)
{
	return _other->Frame != Frame || _other->SpriteType != SpriteType;
}