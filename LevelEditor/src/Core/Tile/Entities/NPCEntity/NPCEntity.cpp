#include "precomp.h"

#include "Core/Tile/Entities/NPCEntity/NPCEntity.h"

inline Ref<TileEntity> NPCEntity::Clone()
{
	return std::make_shared<NPCEntity>(*this);
}
