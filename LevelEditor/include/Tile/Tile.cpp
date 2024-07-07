#include "precomp.h"

#include "Tile/Tile.h"

#include "Shared/Game/NPCDef.h"

#include "Renderer/Renderer.h"

#include "Config/Config.h"

#include "Globals/Globals.h"

ScenicTile::ScenicTile()
{
	SpriteHandle = static_cast<U16>(App::Config::TileConfiguration.SpriteType);
}

void ScenicTile::Render()
{
	Graphics::SpriteType type = static_cast<Graphics::SpriteType>(SpriteHandle);

	RenderQuery query;
	query.Color = { 255,255,255,255 };
	query.Frame = 0;
	query.Position = Coords * App::Config::GRIDCELLSIZE;
	query.Size = Utilities::ivec2(App::Config::GRIDCELLSIZE);
	query.Type = type;

	g_globals.Renderer->Render(query, RenderOrder);

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
		Color color;
		if (bIsWalkable)
		{
			color = { 0, 255, 0, 70 };
		}
		else
		{
			color = { 255, 0, 0, 70 };
		}

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
	query.Position = Coords * App::Config::GRIDCELLSIZE;
	query.Size = Utilities::ivec2(App::Config::GRIDCELLSIZE);
	query.Type = definition.sprite;

	g_globals.Renderer->Render(query, RenderOrder);
}
