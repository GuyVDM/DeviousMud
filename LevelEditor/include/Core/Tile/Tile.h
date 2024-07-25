#pragma once
#include "Core/Core.hpp"

#include "Core/WorldEditor/TileLayer/TileLayer.h"

#include "Shared/Utilities/vec2.hpp"

#include <map>

class TileEntity;

class Tile
{
public:
	const static U32 TILE_VERSION = 1;

	Utilities::ivec2 LocalChunkCoords;
	Utilities::ivec2 ChunkCoords;
	bool             bIsWalkable;

	/// <summary>
	/// Will try and return a TileEntity with the specified layer if it exists.
	/// </summary>
	/// <param name="_layer"></param>
	/// <returns></returns>
	Optional<Ref<TileEntity>> TryGetEntity(const e_SelectedLayer& _layer);

	/// <summary>
	/// Returns false if there are no tile entities bound to this tile.
	/// </summary>
	/// <returns></returns>
	const bool IsEmpty() const;

	/// <summary>
	/// Register a tile entity at the specified layer.
	/// </summary>
	/// <param name="_tileEntity"></param>
	/// <param name="_layer"></param>
	void InsertLayerEntity(Ref<TileEntity> _tileEntity, const e_SelectedLayer& _layer);

	/// <summary>
	/// Remove entitiy at the specified layer.
	/// </summary>
	/// <param name="_layer"></param>
	void RemoveLayerEntity(const e_SelectedLayer& _layer);

	/// <summary>
	/// Render the tile to the screen.
	/// </summary>
	virtual void Render();

	template<class Archive>
	void serialize(Archive& ar, const std::uint32_t version)
	{
		ar(LocalChunkCoords, ChunkCoords, bIsWalkable, m_EntityLayers);
	}

public:
	Tile(const Tile* _other);
	Tile();

private:
	std::map<e_SelectedLayer, Ref<TileEntity>> m_EntityLayers;
};

//Versioning for serialization
CEREAL_CLASS_VERSION(Tile, Tile::TILE_VERSION);



