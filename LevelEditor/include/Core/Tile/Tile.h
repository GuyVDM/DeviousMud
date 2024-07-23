#pragma once
#include "Core/Core.hpp"

#include "Core/WorldEditor/TileLayer/TileLayer.h"

#include "Shared/Game/SpriteTypes.hpp"
#include "Shared/Utilities/vec2.hpp"

#include <map>

/// <summary>
/// Basic tile entity.
/// </summary>
class TileEntity
{
public:
	Graphics::SpriteType SpriteType = Graphics::SpriteType::NONE;
	U32                  Frame      = 0;

	inline const bool operator==(const TileEntity* _other) 
	{
		return _other->Frame == Frame && _other->SpriteType == SpriteType;
	}

	inline const bool operator!=(const TileEntity* _other)
	{
		return _other->Frame != Frame || _other->SpriteType != SpriteType;
	}

	inline virtual Ref<TileEntity> Clone() 
	{
		return std::make_shared<TileEntity>(*this);
	}

public:
	TileEntity() = default;
	virtual ~TileEntity() = default;
};

/// <summary>
/// NPC entity, besides visuals it holds a NPC indentifier and how long until it respawns.
/// </summary>
class NPCEntity : public TileEntity 
{
public:
	U32   NpcId       = 0;
	float RespawnTime = 1.0f;

	inline virtual Ref<TileEntity> Clone() override
	{
		return std::make_shared<NPCEntity>(*this);
	}
};

class Tile
{
public:
	Utilities::ivec2 LocalChunkCoords;
	Utilities::ivec2 ChunkCoords;
	bool             bIsWalkable;

	inline Optional<Ref<TileEntity>> TryGetEntity(const e_SelectedLayer& _layer)
	{
		if(m_EntityLayers.find(_layer) == m_EntityLayers.end())
		{
			return std::nullopt;
		}

		return m_EntityLayers[_layer];
	}

	inline const bool IsEmpty() const
	{
		return m_EntityLayers.size() == 0;
	}

	inline void InsertLayerEntity(Ref<TileEntity> _tileEntity, const e_SelectedLayer& _layer)
	{
		m_EntityLayers[_layer] = _tileEntity;
	}

	inline void RemoveLayerEntity(const e_SelectedLayer& _layer) 
	{
		m_EntityLayers.erase(_layer);
	}

	virtual void Render();

public:
	inline Tile(const Tile* _other) 
	{
		LocalChunkCoords = _other->LocalChunkCoords;
		ChunkCoords = _other->ChunkCoords;
		bIsWalkable = _other->bIsWalkable;

		for(auto&[layer, tileEntt] : _other->m_EntityLayers)
		{
			m_EntityLayers[layer] = tileEntt->Clone();
		}
	}

	inline Tile()
	{
		LocalChunkCoords = Utilities::ivec2(0);
		ChunkCoords      = Utilities::ivec2(0);
		bIsWalkable      = true;
	};

private:
	std::map<e_SelectedLayer, Ref<TileEntity>> m_EntityLayers;
};





