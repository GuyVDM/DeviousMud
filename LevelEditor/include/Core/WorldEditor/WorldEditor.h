#pragma once
#include "Core/Core.hpp"

#include "Core/Tile/Tile.h"
#include "Core/Serialization/JsonSerializable.h"
#include "Core/WorldEditor/Selection/Selection.h"

#include "Shared/Utilities/vec2.hpp"

#include <array>
#include <map>

struct Camera;

namespace Graphics 
{
	enum class SpriteType : U16;
}

constexpr I16 SIZE_CHUNK_TILES = 16;
constexpr I16 TILE_COUNT_CHUNK = SIZE_CHUNK_TILES * SIZE_CHUNK_TILES;

class Chunk : JsonSerializable<Chunk>
{
public:
	IMPLEMENT_SERIALIZABLE;

	inline static const U32 ToTileIndex(const Utilities::ivec2& _localChunkCoords)
	{
		return _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;
	}

	inline bool AddTile(Ref<Tile> _tile)
	{
		const U32 tileIndex = _tile->LocalChunkCoords.x + _tile->LocalChunkCoords.y * SIZE_CHUNK_TILES;

		if (tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if (m_Tiles[tileIndex] == nullptr)
		{
			m_TileCount++;
		}

		m_Tiles[tileIndex] = _tile;
		return true;
	}

	inline bool RemoveTile(const Utilities::ivec2& _localChunkCoords)
	{
		const U32 tileIndex = _localChunkCoords.x + _localChunkCoords.y * SIZE_CHUNK_TILES;

		if (tileIndex < 0 && tileIndex > TILE_COUNT_CHUNK)
		{
			return false;
		}

		if (m_Tiles[tileIndex] != nullptr)
		{
			m_TileCount--;
		}

		m_Tiles[tileIndex] = nullptr;
		return true;
	}

	inline const bool IsValidIndex(const U32& _index) const
	{
		if (_index < 0 || _index >= TILE_COUNT_CHUNK)
			return false;

		return m_Tiles[_index] != nullptr;
	};

	inline const bool IsEmpty() const
	{
		return m_TileCount == 0;
	}

	inline const Optional<Ref<Tile>> TryGetTile(const Utilities::ivec2& _localChunkCoords) const
	{
		const U16 i = Chunk::ToTileIndex(_localChunkCoords);

		if (m_Tiles[i] != nullptr)
		{
			return m_Tiles[i];
		}
		else
		{
			return std::nullopt;
		}
	}

	inline static const Utilities::ivec2 ToChunkCoords(const Utilities::ivec2& _gridCoords)
	{
		return  Utilities::ivec2
		{
			(_gridCoords.x < 0) ? ((_gridCoords.x + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.x / SIZE_CHUNK_TILES),
			(_gridCoords.y < 0) ? ((_gridCoords.y + 1) / SIZE_CHUNK_TILES) - 1 : (_gridCoords.y / SIZE_CHUNK_TILES)
		};;
	}

	inline static const Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _gridCoords)
	{
		return Utilities::ivec2
		{
			(_gridCoords.x % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES,
			(_gridCoords.y % SIZE_CHUNK_TILES + SIZE_CHUNK_TILES) % SIZE_CHUNK_TILES
		};
	}

public:
	Chunk(Utilities::ivec2 _chunkCoords) : m_ChunkCoords(_chunkCoords) {};
	virtual ~Chunk() = default;

public:
	Utilities::ivec2 m_ChunkCoords = { 0 };
	U32 m_TileCount = 0;
	std::array<Ref<Tile>,    TILE_COUNT_CHUNK> m_Tiles;
	std::array<Ref<NPCTile>, TILE_COUNT_CHUNK> m_NPCs;
};

class WorldEditor 
{
public:
	void MoveTileTo(Ref<Tile> _tile, Utilities::ivec2 _gridCoords);

	void Place();

	void Remove();

	void Release();

	void CloneChunk();

	void PasteChunk();

	void RemoveChunk();

	void SerializeHoveredChunk();

	void ClearSelection();

	const bool IsSelectionActive() const;

	const bool IsHoveringOverActiveChunk() const;

	const bool IsTileVisible(const Utilities::ivec2 _gridCoords) const;

public:
	WorldEditor();
	virtual ~WorldEditor() = default;

private:
	Optional<Ref<Tile>> TryGetTile(const Utilities::ivec2& _gridCoords);

	/// <summary>
	/// Try remove tile from attached chunk and return shared pointer.
	/// </summary>
	/// <param name="_gridCoords"></param>
	/// <returns></returns>
	Optional<Ref<Tile>> TryDetachTile(const Utilities::ivec2& _gridCoords);
 
	Optional<Ref<Chunk>> TryGetChunk(const Utilities::ivec2& _gridCoords);

	void HandleShortCuts();

	void Update();

	void PlaceTile(const Utilities::ivec2& _gridCoords);

	void RemoveTile(const Utilities::ivec2& _gridCoords);

	void DragSelectedTiles();

	void TryPlaceSelectedTiles();

	void CreateSelection();

	void CollapseSelection();

	void HandleWandSelection();

	void RecursivelyGenWandSelection(const SubSprite& _sprite, const Utilities::ivec2& _gridCoords, const bool& _bRemoveEntries = false);

	void PickTile();

	void RenderChunkVisuals();

	void PlaceBrushTiles();

	void RemoveBrushTiles();

	void RemoveTilesSelection();

	void Fill();

	void RenderTiles();

	void DrawBrush();

	void DrawSelection();

	const Utilities::ivec2 GetHoveredGridCell() const;

	const Utilities::ivec2 ToChunkCoords(const Utilities::ivec2& _gridCoords) const;

	const Utilities::ivec2 ToLocalChunkCoords(const Utilities::ivec2& _gridCoords) const;

	const Utilities::ivec2 ScreenToGridSpace(const Utilities::ivec2& _screenCoords) const;

	const bool IsValidChunk(const Utilities::ivec2& _chunkCoords) const;

private:
	std::vector<Utilities::ivec2> m_VisitedTiles;

	Utilities::ivec2 m_HoveredGridCell = { 0, 0 };

	SelectionArgs m_SelectionArgs;

	Ref<Camera> m_Camera;

	std::map<Utilities::ivec2, Ref<Chunk>> m_Chunks;

	friend class EditorLayer;
};