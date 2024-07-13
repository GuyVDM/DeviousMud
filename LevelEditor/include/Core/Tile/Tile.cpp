#include "precomp.h"

#include "Core/Tile/Tile.h"

#include "Core/Config/Config.h"
#include "Core/Globals/Globals.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"

#include "Shared/Game/NPCDef.h"

void Tile::Render()
{
	const Utilities::ivec2 chunkCoords = ChunkCoords * SIZE_CHUNK_TILES;
	const Utilities::ivec2 position = (LocalChunkCoords + chunkCoords) * App::Config::GRIDCELLSIZE;

	RenderQuery query;
	query.Color    = { 255,255,255,255 };
	query.Frame    = 0;
	query.Position = position;
	query.Size     = Utilities::ivec2(App::Config::GRIDCELLSIZE);
	query.Type     = sprite;

	g_globals.Renderer->Render(query, 0);

	SDL_Rect rect
	{
		query.Position.x,
		query.Position.y,
		query.Size.x,
		query.Size.y
	};

	//*---------------------------------------
	// Draw whether tiles are walkable or not.
	//
	if (App::Config::SettingsConfiguration.bShowWalkableTiles)
	{
		const Color color = bIsWalkable ? Color(0, 255, 0, 70) 
			                            : Color(255, 0, 0, 70);

		g_globals.Renderer->DrawRect(rect, color);
	}
}

void NPCTile::Render()
{
	NPCDef definition = get_npc_definition(NPCId);

	//*--------------------------------------------------
	// Bit of a hacky way to see whether it's a valid NPC.
	//
	if(definition.idleAnim == e_AnimationType::NO_ANIMATION) 
	{
		return;
	}

	RenderQuery query;
	query.Color = { 255,255,255,255 };
	query.Frame = 0;
	query.Position = (ChunkCoords * SIZE_CHUNK_TILES) * App::Config::GRIDCELLSIZE;
	query.Size = Utilities::ivec2(App::Config::GRIDCELLSIZE) * definition.size;
	query.Type = definition.sprite;

	g_globals.Renderer->Render(query, 5);
}
