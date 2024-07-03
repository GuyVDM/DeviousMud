#include "precomp.h"

#include "WorldEditor/WorldEditor.h"

#include "Camera/Camera.h"

#include "Config/Config.h"

#include "Renderer/Renderer.h"

#include "Globals/Globals.h"

WorldEditor::WorldEditor()
{
	m_Camera = g_globals.Camera;
}

void WorldEditor::Update()
{
	CheckNewHighlight();
	HighlightCurrentTile();
}

void WorldEditor::PlaceTile()
{

}

void WorldEditor::CheckNewHighlight()
{
	using namespace Utilities;

	Utilities::ivec2 mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	const Utilities::ivec2 camPos = m_Camera->Position;

	//*---------------------------
	// Mouse mapped to worldspace.
	// 
	Utilities::ivec2 wsMousePos =
	{
		static_cast<U32>((mousePos.x + camPos.x)),
		static_cast<U32>((mousePos.y + camPos.y))
	};

	const U32 modGridSize = App::Config::GRIDSIZE * m_Camera->Zoom;
	Utilities::ivec2 gridSpaceCoord =
	{
		wsMousePos.x / modGridSize,
		wsMousePos.y / modGridSize
	};

	gridSpaceCoord = gridSpaceCoord;

	//*------------------------------------
	// Account for negative screenspace too
	//*
	{
		if (wsMousePos.x < 0)
		{
			gridSpaceCoord.x * -1;
		}

		if (wsMousePos.y < 0)
		{
			gridSpaceCoord.y * -1;
		}
	}

	m_HoveredGridCell = gridSpaceCoord;
	DEVIOUS_LOG("Gridspace is: " << gridSpaceCoord.x << " , " << gridSpaceCoord.y);
}

void WorldEditor::HighlightCurrentTile()
{
	static float elapsedTime = 1.0f;

	elapsedTime += App::Config::Configuration::GetDT();

	const float frequency = 3.0f;
	const U8 alpha = static_cast<U8>(30 + (std::sinf(frequency * elapsedTime) + 1) * 30.0f);
	const Color col = { 255, 255, 0, alpha };

	const SDL_Rect rect
	{
		(m_HoveredGridCell.x * App::Config::GRIDSIZE),
		(m_HoveredGridCell.y * App::Config::GRIDSIZE),
		App::Config::GRIDSIZE,
		App::Config::GRIDSIZE
	};

	g_globals.Renderer->DrawRectViewProjection(rect, col);
	
}


