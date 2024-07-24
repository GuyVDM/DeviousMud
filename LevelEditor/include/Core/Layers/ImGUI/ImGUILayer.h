#pragma once

#include "Core/Core.hpp"

#include "Core/Layers/Layer.h"

struct SDL_Window;
struct SDL_Renderer;

enum class e_ImGuiIconType 
{
	ICON_FILL = 0x00,
	ICON_BRUSH,
	ICON_PATH,
	ICON_RESET,
	ICON_PICKER,
	ICON_GRID,
	ICON_SELECT,
	ICON_WAND,
	ICON_DRAG,
	ICON_LAYER,

	ICON_INFO,
	ICON_WARNING,
	ICON_ERROR,
	ICON_EVENT
};

class ImguiIcons 
{
public:
	using IconMap = std::unordered_map<e_ImGuiIconType, const char*>;

	const static IconMap GetIconMap() 
	{
		const IconMap iconMap =
		{
			{ e_ImGuiIconType::ICON_FILL,   "icons/fill.png"    },
			{ e_ImGuiIconType::ICON_BRUSH,  "icons/brush.png"   },
			{ e_ImGuiIconType::ICON_PATH,   "icons/path.png"    },
			{ e_ImGuiIconType::ICON_RESET,  "icons/reset.png"   },
			{ e_ImGuiIconType::ICON_PICKER, "icons/picker.png"  },
			{ e_ImGuiIconType::ICON_GRID,   "icons/grid.png"    },
			{ e_ImGuiIconType::ICON_SELECT, "icons/select.png"  },
			{ e_ImGuiIconType::ICON_WAND,   "icons/wand.png"    },
			{ e_ImGuiIconType::ICON_DRAG,   "icons/cursor.png"  },
			{ e_ImGuiIconType::ICON_LAYER,  "icons/layer.png"   },

			{ e_ImGuiIconType::ICON_INFO,   "icons/info.png"    },
			{ e_ImGuiIconType::ICON_EVENT,  "icons/event.png"   },
			{ e_ImGuiIconType::ICON_ERROR,  "icons/warning.png" },
			{ e_ImGuiIconType::ICON_WARNING,"icons/error.png"   }
		};

		return iconMap;
	}
};

class ImGUILayer : public Layer 
{
public:
	IMPLEMENT_LAYER_FUNCTIONS;

public:
	ImGUILayer(SDL_Window*, SDL_Renderer*);
	virtual ~ImGUILayer();

private:
	void LoadImGuiIcons();

	void DestroyImGuiIcons();

	void DrawImGUI();

	void DrawModeSelectorWindow();

	void DrawMenuBar();

	void DrawTextureSelectorWindow();

	void DrawSettingsWindow();

	void DrawRightClickMenu() const;

	void DrawScenicSettings();

	void DrawNPCSettings();

	void DrawLayerWindow();

	void DrawLoggingWindow();

private:
	SDL_Renderer* m_Renderer;

	std::map<e_ImGuiIconType, SDL_Texture*> m_Icons;

	U8   m_logFilterFlags   = 0xFF;
	bool m_bCtrlPressed     = false;
	bool m_bLayerWindowOpen = true;
};