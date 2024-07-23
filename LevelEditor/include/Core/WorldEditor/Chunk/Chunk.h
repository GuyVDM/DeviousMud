#pragma once
#include "Core/Core.hpp"
#include "Core/Serialization/JsonSerializable.h"

class Tile;
class TileEntity;

enum class e_SelectedLayer : U8;

constexpr I16 SIZE_CHUNK_TILES = 16;
constexpr I16 TILE_COUNT_CHUNK = SIZE_CHUNK_TILES * SIZE_CHUNK_TILES;

class Chunk : JsonSerializable<Chunk>
{
public:
	IMPLEMENT_SERIALIZABLE;

	/// <summary>
	/// Converts the local coordinates relative to the chunk into a valid index pointing
	/// towards the appropriate pointer.
	/// </summary>
	/// <param name="_localChunkCoords"></param>
	/// <returns></returns>
	static const U32 ToTileIndex(const Utilities::ivec2& _localChunkCoords);

	/// <summary>
	/// Adds the tile entity to the specified tile if the specified local chunk coords are appropriate.
	/// local chunk coords, on each axis, goes from 0 to SIZE_CHUNK_TILES - 1.
	/// If the local chunk coords are below 0 or => SIZE_CHUNK_TILES, then the local coordinates are considered invalid.
	/// </summary>
	/// <param name="_localChunkCoords"></param>
	/// <param name="_tileEntity"></param>
	/// <param name="_layer"></param>
	/// <returns></returns>
	bool AddToTile(const Utilities::ivec2& _localChunkCoords, Ref<TileEntity> _tileEntity, const e_SelectedLayer& _layer);

	/// <summary>
	/// Add a tile will all layers directly to the chunk.
	/// </summary>
	/// <param name="_tile"></param>
	bool AddTile(Ref<Tile> _tile);

	/// <summary>
	/// Removes the tile entity from the specified tile if the specified local chunk coords are appropriate.
	/// local chunk coords, on each axis, goes from 0 to SIZE_CHUNK_TILES - 1.
	/// If the local chunk coords are below 0 or => SIZE_CHUNK_TILES, then the local coordinates are considered invalid.
	/// </summary>
	/// <param name="_localChunkCoords"></param>
	/// <param name="_tileEntity"></param>
	/// <param name="_layer"></param>
	/// <returns></returns>
	bool RemoveFromTile(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer);

	/// <summary>
	/// Almost the same as remove, but in this instance, it removes the tile entity from the tile.
	/// And returns the object, so you can for example, move it to another tile.
	/// </summary>
	/// <param name="_localChunkCoords"></param>
	/// <param name="_layer"></param>
	/// <returns></returns>
	Optional<Ref<TileEntity>> DetachFromTile(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer);

	/// <summary>
	/// Tries to return a tile entity that is attached to the tile based on the given layertype.
	/// </summary>
	/// <param name="_localChunkCoords"></param>
	/// <param name="_layer"></param>
	/// <returns></returns>
	const Optional<Ref<TileEntity>> TryGetTileEntity(const Utilities::ivec2& _localChunkCoords, const e_SelectedLayer& _layer);

	/// <summary>
	/// Whether the given index does point to a existing tile.
	/// </summary>
	/// <param name="_index"></param>
	/// <returns></returns>
	const bool IsValidIndex(const U32& _index) const;

	/// <summary>
	/// Whether this chunk has any registered tiles or not.
	/// </summary>
	/// <returns></returns>
	const bool IsEmpty() const;

	/// <summary>
	/// Transforms gridspace coordinates to chunk coordinates.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	static const Utilities::ivec2 ToChunkCoords(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Transforms gridspace coordinates to coodinates relative to a chunk that's within the same region.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	static const Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _gridCoords);

public:
	Chunk(Utilities::ivec2 _chunkCoords) : m_ChunkCoords(_chunkCoords) {};
	virtual ~Chunk() = default;

public:
	Utilities::ivec2 m_ChunkCoords = { 0 };
	U32 m_TileCount = 0;
	std::array<Ref<Tile>, TILE_COUNT_CHUNK> m_Tiles;
};