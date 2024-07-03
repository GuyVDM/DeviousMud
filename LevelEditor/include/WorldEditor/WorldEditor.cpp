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
	HighlightCurrentTile();
}

Utilities::ivec2 WorldEditor::GetHoveredGridCell()
{
	Utilities::ivec2 mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	Utilities::ivec2 worldPos
	{
		static_cast<int>((mousePos.x / m_Camera->Zoom) + m_Camera->Position.x),
		static_cast<int>((mousePos.y / m_Camera->Zoom) + m_Camera->Position.y)
	};

	if (worldPos.x < 0)
		worldPos.x -= App::Config::GRIDSIZE - 1;
	if (worldPos.y < 0)
		worldPos.y -= App::Config::GRIDSIZE - 1;

	Utilities::ivec2 gridPos =
	{
		static_cast<int>(worldPos.x / App::Config::GRIDSIZE),
		static_cast<int>(worldPos.y / App::Config::GRIDSIZE)
	};

	return gridPos;
}

void WorldEditor::HighlightCurrentTile()
{
	static float elapsedTime = 0.f;

	elapsedTime += App::Config::Configuration::GetDT();

	m_HoveredGridCell = GetHoveredGridCell();

	const SDL_Rect rect
	{
		m_HoveredGridCell.x * App::Config::GRIDSIZE,
		m_HoveredGridCell.y * App::Config::GRIDSIZE,
		App::Config::GRIDSIZE,
		App::Config::GRIDSIZE
	};

	const Color col = { 255, 255, 0, DMath::Occilate<U8>(30.0f, 100.0f, 3.0f, elapsedTime)};
	g_globals.Renderer->DrawRect(rect, col, 10);
	
}


