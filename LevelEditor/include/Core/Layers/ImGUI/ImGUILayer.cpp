#include "precomp.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/FileHandler/FileHandler.h"
#include "Core/Globals/Globals.h"
#include "Core/Layers/ImGUI/ImGUILayer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"
#include "Core/Editor/Editor.h"
#include "Core/WorldEditor/TileLayer/TileLayer.h"

#include "Shared/Game/SpriteTypes.hpp"

static void ImGuiGetSpriteUVCoords(const Graphics::SpriteType _type, const U32& _frame, ImVec2& _uv0, ImVec2& _uv1) 
{
    Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(_type);

    if(!optSprite.has_value())
    {
        _uv0 = { 0.0f, 0.0f };
        _uv1 = { 0.0f, 0.0f };
        return;
    }

    Sprite s = optSprite.value();
    
    const float frameWidth  = 1.0f / static_cast<float>(s.Columns);
    const float frameHeight = 1.0f / static_cast<float>(s.Rows);
    
    const U32 targetColumn = _frame % s.Columns;
    const U32 targetRow    = _frame / s.Columns;

    _uv0 =
    {
        targetColumn * frameWidth,
        targetRow    * frameHeight
    };

    _uv1 =
    {
        _uv0.x + frameWidth,
        _uv0.y + frameHeight
    };
}

ImGUILayer::ImGUILayer(SDL_Window* _window, SDL_Renderer* _renderer)
{
    m_Renderer = _renderer;

    //*----------------------------
    // Initialise IMGUI
    //*
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(App::Config::s_FontPath, 16.0f);
        io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        ImGui_ImplSDL2_InitForSDLRenderer(_window, _renderer);
        ImGui_ImplSDLRenderer2_Init(_renderer);
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Border]              = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_WindowBg]            = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_TitleBg]             = { 0.44f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_TitleBgActive]       = { 0.44f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_TitleBgCollapsed]    = { 0.44f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_FrameBg]             = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_FrameBgHovered]      = { 0.34f, 0.34f, 0.34f, 1.0f };
    style.Colors[ImGuiCol_FrameBgActive]       = { 0.30f, 0.30f, 0.30f, 1.0f };
    style.Colors[ImGuiCol_TabHovered]          = { 0.29f, 0.29f, 0.29f, 1.0f };
    style.Colors[ImGuiCol_Tab]                 = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_TabSelected]         = { 0.34f, 0.34f, 0.34f, 1.0f };
    style.Colors[ImGuiCol_TabSelectedOverline] = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_ResizeGripHovered]   = { 0.5f,  0.5f,  0.0f,  1.0f };
    style.Colors[ImGuiCol_ResizeGripActive]    = { 0.7f,  0.7f,  0.0f,  1.0f };
    style.Colors[ImGuiCol_MenuBarBg]           = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_Button]              = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_ButtonActive]        = { 0.29f, 0.29f, 0.29f, 1.0f };
    style.Colors[ImGuiCol_ButtonHovered]       = { 0.34f, 0.34f, 0.34f, 1.0f };
    style.Colors[ImGuiCol_SliderGrab]          = { 0.34f, 0.34f, 0.34f, 1.0f };
    style.Colors[ImGuiCol_SliderGrabActive]    = { 0.29f, 0.29f, 0.29f, 1.0f };
    style.Colors[ImGuiCol_Header]              = { 0.29f, 0.29f, 0.29f, 1.0f };
    style.Colors[ImGuiCol_HeaderActive]        = { 0.29f, 0.29f, 0.29f, 1.0f };
    style.Colors[ImGuiCol_HeaderHovered]       = { 0.29f, 0.29f, 0.29f, 1.0f };

    LoadImGuiIcons();
}

ImGUILayer::~ImGUILayer()
{
    DestroyImGuiIcons();

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

const bool ImGUILayer::HandleEvent(const SDL_Event& _event)
{
    ImGui_ImplSDL2_ProcessEvent(&_event);

    switch(_event.type) 
    {
        case SDL_KEYDOWN:
        {
            if (_event.key.keysym.sym == SDLK_LCTRL || _event.key.keysym.sym == SDLK_RCTRL)
            {
                m_bCtrlPressed = true;
            }
        }
        break;

        case SDL_KEYUP:
        {
            if (_event.key.keysym.sym == SDLK_LCTRL || _event.key.keysym.sym == SDLK_RCTRL)
            {
                m_bCtrlPressed = false;
            }
        }
        break;
    }

    const ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void ImGUILayer::Update()
{
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();

    ImGui::NewFrame();

    DrawImGUI();

    ImGui::EndFrame();
    ImGui::Render();
}

void ImGUILayer::LoadImGuiIcons()
{
    for (auto& [type, path] : ImguiIcons::GetIconMap())
    {
        SDL_Surface* surface = IMG_Load(path);

        if (!surface)
        {
            DEVIOUS_ERR("Failed to load surface: " << path);
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_Renderer, surface);

        if (!texture)
        {
            DEVIOUS_ERR("Failed to create texture from surface loaded from: " << path);
            return;
        }

        m_Icons[type] = texture;
        SDL_FreeSurface(surface);
    }
}

void ImGUILayer::DestroyImGuiIcons()
{
    for(auto&[type, texture] : m_Icons) 
    {
        SDL_DestroyTexture(texture);
    }

    m_Icons.clear();
}

void ImGUILayer::DrawImGUI()
{
    if (ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode))
    {
        DrawMenuBar();
        DrawModeSelectorWindow();
        DrawTextureSelectorWindow();
        DrawSettingsWindow();
        DrawRightClickMenu();
        DrawLayerWindow();
        DrawLoggingWindow();
        DrawNewMapPopup();
    }
}

void ImGUILayer::DrawModeSelectorWindow()
{
    using namespace App::Config;

    const auto ImGuiDrawButton = [](const char* _label, const ImVec2& _parentPos, const float _yOffset, SDL_Texture* _tex, const bool bIsSelected = false)
    {
        constexpr ImVec2 buttonSize = { 50.0f, 30.0f };
        constexpr ImVec2 buttonSizeHalfExtends = { buttonSize.x / 2.0f, buttonSize.y / 2.0f };

        constexpr ImVec2 iconSize   = { 20.0f, 20.0f };
        constexpr ImVec2 iconSizeHalfExtends = { iconSize.x / 2.0f, iconSize.y / 2.0f };

        constexpr ImVec2 iconOffset = { buttonSizeHalfExtends.x - iconSizeHalfExtends.x, buttonSizeHalfExtends.y - iconSizeHalfExtends.y };

        bool bIsPressed = false;
        
        ImGui::SetCursorPosY(_yOffset);

        if(bIsSelected) 
        {
            ImGui::PushStyleColor(ImGuiCol_Button,        { 0.5f, 0.23f, 0.23f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.6f, 0.23f, 0.23f, 1.0f });

            bIsPressed = ImGui::Button(_label, buttonSize);
            ImGui::PopStyleColor(2);
        }
        else 
        {
            bIsPressed = ImGui::Button(_label, buttonSize);
        }

        ImVec2 buttonPos = ImGui::GetItemRectMin();
        buttonPos.x -= _parentPos.x;
        buttonPos.y -= _parentPos.y;
        buttonPos.x += iconOffset.x;
        buttonPos.y += iconOffset.y;

        ImGui::SetCursorPos({buttonPos.x, buttonPos.y});
        
        ImGui::Image(_tex, iconSize);

        return bIsPressed;
    };

    constexpr ImVec2 windowSize = { 64.0f, 350.0f };

    constexpr U32 viewportFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    ImGui::SetNextWindowSize(windowSize);
    
    ImGui::Begin("##Tools", nullptr, viewportFlags);
    {
        const ImVec2 menuPos = ImGui::GetItemRectMin();

        if (ImGuiDrawButton("##Drag", menuPos, 30.0f, m_Icons[e_ImGuiIconType::ICON_DRAG],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_DRAG))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_DRAG;
        }

        if (ImGuiDrawButton("##Single", menuPos, 65.0f, m_Icons[e_ImGuiIconType::ICON_BRUSH], 
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_BRUSH))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_BRUSH;
        }

        if(ImGuiDrawButton("##Fill", menuPos, 100.0f, m_Icons[e_ImGuiIconType::ICON_FILL],
           TileConfiguration.InteractionMode == e_InteractionMode::MODE_FILL))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_FILL;
        } 

        if (ImGuiDrawButton("##Path", menuPos, 135.0f, m_Icons[e_ImGuiIconType::ICON_PATH], 
            SettingsConfiguration.bShowWalkableTiles))
        {
            SettingsConfiguration.bShowWalkableTiles = !SettingsConfiguration.bShowWalkableTiles;
        }

        if (ImGuiDrawButton("##Reset", menuPos, 170.0f, m_Icons[e_ImGuiIconType::ICON_RESET]))
        {
            g_globals.Camera->Reset();
        }

        if (ImGuiDrawButton("##ChunkGrid", menuPos, 205.0f, m_Icons[e_ImGuiIconType::ICON_GRID],
            SettingsConfiguration.bRenderChunkVisuals))
        {
            SettingsConfiguration.bRenderChunkVisuals = !SettingsConfiguration.bRenderChunkVisuals;
        }

        if (ImGuiDrawButton("##Picker", menuPos, 240.0f, m_Icons[e_ImGuiIconType::ICON_PICKER],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_PICKER))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_PICKER;
        }

        if (ImGuiDrawButton("##Select", menuPos, 275.0f, m_Icons[e_ImGuiIconType::ICON_SELECT],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_SELECTION))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_SELECTION;
        }

        if (ImGuiDrawButton("##Wand", menuPos, 310.0f, m_Icons[e_ImGuiIconType::ICON_WAND],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_WAND))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_WAND;
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}

void ImGUILayer::DrawMenuBar()
{
    using namespace App::Config;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", "Ctrl+E"))
        {
            ImGui::MenuItem("(file)", NULL, false, false);

            if (ImGui::MenuItem("Load Map", "CTRL+L")) g_globals.WorldEditor->LoadMap();
            if (ImGui::MenuItem("Save Map", "CTRL+S")) g_globals.WorldEditor->QuickSaveMap();
            if (ImGui::MenuItem("New  Map", "")) 
            {
                m_bNewMapPopup = true;
            }

            ImGui::Separator();

            ImGui::MenuItem("(export)", NULL, false, false);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::MenuItem("Reset", "CTRL+R")) g_globals.Camera->Reset();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Show Layers", "")) m_bLayerWindowOpen = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("(toggles)", NULL, false, false);
            if (ImGui::MenuItem("Toggle NPC Parameters on Hover", "")) SettingsConfiguration.bShowNpcParams = !SettingsConfiguration.bShowNpcParams;

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    if(ImGui::BeginSecondMenuBar()) 
    {
        ImGui::Text("Brushsize:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt2("##Brush", &App::Config::SettingsConfiguration.BrushSize.x, 0);
    }
    ImGui::EndSecondMenuBar();
}

void ImGUILayer::DrawTextureSelectorWindow()
{
    using namespace App::Config;

    if (TileConfiguration.CurrentLayer == e_SelectedLayer::LAYER_NPC)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    if (ImGui::Begin("Choose Texture", 0, ImGuiWindowFlags_NoCollapse))
    {
        const Graphics::SpriteType spriteType = TileConfiguration.Sprite.SpriteType;

        Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(spriteType).value();

        if (optSprite.has_value())
        {
            const Sprite& sprite = optSprite.value();

            constexpr ImVec2 buttonSize = { 84.0f, 84.0f };

            I32 maxItemsOnLine = static_cast<I32>(ImGui::GetWindowWidth() / buttonSize.x) + 1; 
            I32 itemCountOnCurrentLine = 0;

            for (I32 i = 0; i < static_cast<I32>(sprite.FrameCount); i++)
            {
                itemCountOnCurrentLine++;
                if(itemCountOnCurrentLine % maxItemsOnLine != 0)
                {
                    ImGui::SameLine(0.0f, 0.0f);
                }
                else
                {
                    itemCountOnCurrentLine = 1;
                }

                ImVec2 uv0, uv1;
                ImGuiGetSpriteUVCoords(spriteType, i, uv0, uv1);

                const std::string buttonID = "##SelectTile" + std::to_string(i);
                if (ImGui::ImageButton(buttonID.c_str(), sprite.Texture, buttonSize, uv0, uv1))
                {
                    TileConfiguration.Sprite = SubSprite(spriteType, i);
                }
            }
        }
    }

    ImGui::PopStyleColor();

    ImGui::End();
}

void ImGUILayer::DrawSettingsWindow()
{
    using namespace App::Config;

    switch (TileConfiguration.CurrentLayer)
    {
        case e_SelectedLayer::LAYER_NPC:
        {
            DrawNPCSettings();
        }
        break;

        default:
        {
            DrawScenicSettings();
        }
        break;
    }
}

void ImGUILayer::DrawRightClickMenu() const
{
    using namespace App::Config;

    static bool bIsOpened = false;
    static U32  framesOpened = 0;

    if (!bIsOpened && m_bCtrlPressed)
    {
        ImVec2 offset = ImVec2(10.0f, 20.0f);
        ImVec2 position = 
        {
            ImGui::GetMousePos().x - offset.x,
            ImGui::GetMousePos().y - offset.y
        };

        ImGui::SetNextWindowPos(position);
    }

    if (m_bCtrlPressed)
    {
        if (ImGui::BeginPopupContextVoid("ViewportContextMenu", ImGuiPopupFlags_MouseButtonRight))
        {
            bIsOpened = true;

            ImGui::MenuItem("(camera)", NULL, false, false);
            if (ImGui::MenuItem("Reset Camera")) g_globals.Camera->Reset();
            ImGui::Separator();

            ImGui::MenuItem("(chunk)", NULL, false, false);
            ImGui::Separator();

            if (!TileConfiguration.ChunkClipboard.expired())
            {
                if (ImGui::MenuItem("Paste Chunk")) g_globals.WorldEditor->PasteChunk();
            }

            if (ImGui::MenuItem("Clone Chunk"))   g_globals.WorldEditor->CloneChunk();  
            if (ImGui::MenuItem("Delete Chunk"))  g_globals.WorldEditor->RemoveChunk();

            if(g_globals.WorldEditor->IsSelectionActive()) 
            {
                ImGui::Separator();
                ImGui::MenuItem("(selection)", NULL, false, false);
                if (ImGui::MenuItem("Clear Selection")) g_globals.WorldEditor->ClearSelection();
                if (ImGui::MenuItem("Make Walkable")) g_globals.WorldEditor->SetSelectionNavigatable(true);
                if (ImGui::MenuItem("Make Obstacle")) g_globals.WorldEditor->SetSelectionNavigatable(false);
            }

            //*------------------------------------------------------------
            // For some reason the first two frames the menu is opened
            // The window does not think it's hovered despite being hovered.
            // Got to find a proper fix for this one later.
            if (!ImGui::IsWindowHovered() && bIsOpened)
            {
                framesOpened++;

                if (framesOpened > 2)
                {
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
        else
        {
            bIsOpened = false;
            framesOpened = 0;
        }
    }
}

void ImGUILayer::DrawScenicSettings()
{
    using namespace App::Config;

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Selected Brush", 0, flags))
    {
        const     ImVec2 windowSize = ImGui::GetWindowSize();
        constexpr ImVec2 previewSize = { 84.0f, 84.0f };

        ImGui::SameLine((windowSize.x * 0.5f) - previewSize.x * 0.5f);

        const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        const float verticalOffset = contentRegionAvailable.y * 0.5f - 42.0f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

        Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(TileConfiguration.Sprite.SpriteType);

        if (!optSprite.has_value())
        {
            return;
        }

        ImVec2 uv0, uv1;
        ImGuiGetSpriteUVCoords(TileConfiguration.Sprite.SpriteType, TileConfiguration.Sprite.Frame, uv0, uv1);

        ImGui::Image(optSprite.value().Texture, { 84.0f, 84.0f }, uv0, uv1);
    }
    ImGui::End();
}

void ImGUILayer::DrawNPCSettings()
{
    using namespace App::Config;
    if (ImGui::Begin("NPC Window:", 0, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Spacing();

        ImGui::SeparatorText("NPC Settings:");
        ImGui::Text("Id:");
        if (ImGui::InputInt("##NPCId", &TileConfiguration.NPCid))
        {
            TileConfiguration.NPCid = std::clamp<I32>(TileConfiguration.NPCid, 0, INT_FAST32_MAX);
            TileConfiguration.SelectedNPC = get_npc_definition(TileConfiguration.NPCid);
            TileConfiguration.Sprite.SpriteType = TileConfiguration.SelectedNPC.sprite;
        }

        ImGui::Spacing();

        ImGui::Text("Respawn Time:");
        ImGui::InputFloat("##RespawnTime", &TileConfiguration.NPCRespawnTime, 0.5f, 1.0f);
        ImGui::Spacing();

        ImGui::Spacing();

        ImGui::SeparatorText("Preview:");

        ImGui::Text("Name:");
        ImGui::SameLine();
        ImGui::TextColored({ 0, 255, 0, 255 }, TileConfiguration.SelectedNPC.name.c_str());

        ImGui::Text("Combat Level:");
        ImGui::SameLine();

        const std::string levelHeader = "Level-" + std::to_string(TileConfiguration.SelectedNPC.combatLevel);
        ImGui::TextColored({ 0, 255, 0, 255 }, levelHeader.c_str());

        ImGui::Text("Size:");
        ImGui::SameLine();
        ImGui::TextColored({ 0, 255, 0, 255 }, std::to_string(TileConfiguration.SelectedNPC.size).c_str());

        ImGui::Text("Sprite Id:");
        ImGui::SameLine();
        ImGui::TextColored({ 0, 255, 0, 255 }, std::to_string(static_cast<U32>(TileConfiguration.SelectedNPC.sprite)).c_str());

        //*--------------------------------
        // Render sprite preview of the NPC
        //
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 previewSize = { 84.0f, 84.0f };

            ImGui::SameLine((windowSize.x * 0.5f) - previewSize.x * 0.5f);

            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
            const float verticalOffset = contentRegionAvailable.y * 0.5f - 42.0f;
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

            Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(TileConfiguration.SelectedNPC.sprite);
            if (optSprite.has_value())
            {
                ImVec2 uv0, uv1 = ImVec2(0.0f, 0.0f);
                ImGuiGetSpriteUVCoords(TileConfiguration.SelectedNPC.sprite, TileConfiguration.Sprite.Frame, uv0, uv1);

                ImGui::Image(optSprite.value().Texture, { 84.0f, 84.0f }, uv0, uv1);
            }
        }
    }

    ImGui::End();
}

void ImGUILayer::DrawLayerWindow()
{
    constexpr ImVec2 windowSize = { 200.0f, 220.0f };
    constexpr ImVec2 buttonSize = { windowSize.x - 42.0f, 65.0f };

    const auto DrawImGuiLayerButton = [this, buttonSize, windowSize](const e_SelectedLayer& _layer)
    {
            using namespace App::Config;

            const std::string LayerName   = Layers::GetLayerName(_layer);
            const I32         buttonIndex = static_cast<I32>(_layer);
            const ImVec2      buttonPos   = ImVec2
            (
                0.0f,
                23.0f + ((buttonSize.y) * static_cast<float>(buttonIndex))
            );

            //*--------------------
            // Create layer button.
            //
            {
                ImGui::SetCursorPos(buttonPos);

                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                const std::string buttonId = "##LayerButton" + std::to_string(buttonIndex);
                if(ImGui::Button(buttonId.c_str(), buttonSize)) 
                {
                    TileConfiguration.CurrentLayer = _layer;
                    TileConfiguration.Sprite = SubSprite(Layers::LayerToSprite(_layer), 0);
                }

                ImGui::PopStyleColor(3);
            }

            //*--------------------
            // Display selection fill & border
            //         
            if (_layer == TileConfiguration.CurrentLayer)
            {
                constexpr float scrollBarsizePx = 16;

                const ImVec2 pos  = ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMin().y);
                const ImVec2 pos1 = ImVec2(pos.x + ImGui::GetWindowWidth() - scrollBarsizePx, pos.y + buttonSize.y);

                ImDrawList* drawList = ImGui::GetWindowDrawList();

                constexpr ImU32 fillColor = IM_COL32(20, 80, 128, 255);
                drawList->AddRectFilled(pos, pos1, fillColor);

                constexpr ImU32 outlineColor = IM_COL32(0, 120, 215, 255);
                drawList->AddRect(pos, pos1, outlineColor, 0.0f, 0, 1.0f);
            }

            //*-------------------------------
            // Render layer icon on the button
            //
            {
                constexpr ImVec2 buttonIconSize = { 30.0f, 30.0f };
                const     ImVec2 buttonIconPos =
                {
                    10.0f,
                    buttonPos.y + (buttonSize.y / 2.0f) - (buttonIconSize.y / 2.0f)
                };

                ImGui::SetCursorPos(buttonIconPos);
                ImGui::Image(m_Icons[e_ImGuiIconType::ICON_LAYER], buttonIconSize);
            }

            //*-------------------------------
            // Render text next to the icon
            //
            {
                const ImVec2 buttonTextSize = ImGui::CalcTextSize(LayerName.c_str());
                const ImVec2 buttonTextPos =
                {
                    50.0f,
                    buttonPos.y + (buttonSize.y * 0.5f) - (buttonTextSize.y * 0.5f)
                };

                ImGui::SetCursorPos(buttonTextPos);
                ImGui::Text(LayerName.c_str());
            }

            //*-------------------------------
            // Render checkbox to the right of the button
            //
            {
                const std::string checkboxLabel = "##LayerCheckbox" + std::to_string(buttonIndex);

                const ImVec2 checkboxSize = { 42.0f, 42.0f };
                const ImVec2 checkboxPos =
                {
                    windowSize.x - checkboxSize.x,
                    buttonPos.y + (buttonSize.y * 0.5f) - (checkboxSize.y * 0.25f)
                };

                const U32 bitPos = static_cast<U32>(_layer);
                bool bIsLayerVisible = U32IsBitSet(TileConfiguration.VisibleLayerFlags, bitPos);

                ImGui::SetCursorPos(checkboxPos);
                if (ImGui::Checkbox(checkboxLabel.c_str(), &bIsLayerVisible))
                {
                    U32FlipBit(App::Config::TileConfiguration.VisibleLayerFlags, bitPos);
                }
            }
    };

    ImGui::SetNextWindowSize(windowSize);
    
    constexpr U32 viewportFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;

    if (m_bLayerWindowOpen)
    {
        if (ImGui::Begin("Layers", &m_bLayerWindowOpen, viewportFlags))
        {
            for (U32 i = 0; i < Layers::s_LayerCount; i++)
            {
                const e_SelectedLayer layer = static_cast<e_SelectedLayer>(i);
                DrawImGuiLayerButton(layer);
            }
        }

        ImGui::End();
    }
}

void ImGUILayer::DrawLoggingWindow()
{
    static char logFilterBuf[256] = "";

    const auto DrawLogFilterButton = [this](const e_LogType& _logType)
    {
        ImGui::SameLine();

        e_ImGuiIconType icon = e_ImGuiIconType::ICON_INFO;

        float  multiplier = 4.0f;

        switch (_logType)
        {
            case e_LogType::TYPE_ERROR:
                icon = e_ImGuiIconType::ICON_ERROR;
                multiplier = 1.0f;
                break;

            case e_LogType::TYPE_WARN:
                icon = e_ImGuiIconType::ICON_WARNING;
                multiplier = 2.0f;
                break;

            case e_LogType::TYPE_EVENT:
                icon = e_ImGuiIconType::ICON_EVENT;
                multiplier = 3.0f;
                break;
        }

        constexpr ImVec2 filterButtonSize = ImVec2(50.0f, 20.0f);

        ImVec2 buttonPos = ImVec2((ImGui::GetWindowSize().x - (filterButtonSize.x + 5.0f) * multiplier), 0.0f);

        ImGui::SetCursorPos(buttonPos);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

        const std::string label = "##LogFilter" + std::to_string(multiplier);
        if (ImGui::Button(label.c_str(), filterButtonSize))
        {
            m_logFilterFlags ^= _logType;
        }

        ImGui::PopStyleVar();

        constexpr ImVec2 filterIconSize = ImVec2(16.0f, 16.0f);
        const     ImVec2 iconPos = ImVec2(buttonPos.x + 4.0f, buttonPos.y + (filterButtonSize.y * 0.5f) - (filterIconSize.y * 0.5f));

        ImVec4 tintColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        if((m_logFilterFlags & _logType) == 0) 
        {
            tintColor = { 0.6f, 0.6f, 0.6f, 0.6f };
        }

        ImGui::SetCursorPos(iconPos);
        ImGui::Image(m_Icons[icon], filterIconSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), tintColor);

        const ImVec2 textPos = ImVec2(buttonPos.x + filterIconSize.x + 8.0f, buttonPos.y + 2.0f);
        ImGui::SetCursorPos(textPos);
        ImGui::Text(std::to_string(Logger::GetMessagesCountOfLogType(_logType)).c_str());
    };

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
    if (ImGui::Begin("Logger"), 0, windowFlags)
    {
        if (ImGui::Button("Clear")) Logger::ClearLog();
        ImGui::SameLine();
        ImGui::InputText("Filter", logFilterBuf, IM_ARRAYSIZE(logFilterBuf));
        ImGui::SameLine();

        const ImVec2 filterButtonContentSize = ImVec2(ImGui::GetContentRegionAvail().x, 32.0f);

        ImGui::BeginChild("Filterbuttons", filterButtonContentSize, 0, windowFlags);
        DrawLogFilterButton(e_LogType::TYPE_INFO);
        DrawLogFilterButton(e_LogType::TYPE_EVENT);
        DrawLogFilterButton(e_LogType::TYPE_WARN);
        DrawLogFilterButton(e_LogType::TYPE_ERROR);
        ImGui::EndChild();

        ImGui::Spacing();

        constexpr ImVec2 minContentSize = { 0.0f, 0.0f };
        const     ImVec2 maxContentSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

        if (maxContentSize.y > 0.0f)
        {
            ImGui::BeginChild("ScrollableArea", maxContentSize, true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysVerticalScrollbar);

            for (Message& msg : Logger::GetMessages())
            {
                if (std::strlen(logFilterBuf) > 0)
                {
                    if (std::strstr(msg.Contents.c_str(), logFilterBuf) == nullptr)
                    {
                        continue;
                    }
                }

                //If were filtering these log types out.
                if((m_logFilterFlags & msg.LogType) == 0) 
                {
                    continue;
                }

                e_ImGuiIconType iconType = e_ImGuiIconType::ICON_INFO;

                switch (msg.LogType)
                {
                    case e_LogType::TYPE_ERROR:
                        iconType = e_ImGuiIconType::ICON_ERROR;
                        break;

                    case e_LogType::TYPE_WARN:
                        iconType = e_ImGuiIconType::ICON_WARNING;
                        break;

                    case e_LogType::TYPE_EVENT:
                        iconType = e_ImGuiIconType::ICON_EVENT;
                        break;
                }

                ImGui::SetCursorPos(ImVec2(25.0f, ImGui::GetCursorPos().y));

                constexpr ImVec4 msgColor = { 255, 255, 255, 255 };
                ImGui::TextColored(msgColor, msg.Contents.c_str());
                ImVec2 prevCursorPos = ImGui::GetCursorPos();

                constexpr ImVec2 iconSize = { 16.0f, 16.0f };
                ImGui::SetCursorPos(ImVec2(5.0f, prevCursorPos.y - (iconSize.y * 1.25f)));
                ImGui::Image(m_Icons[iconType], iconSize);
                ImGui::SetCursorPos(prevCursorPos);

            }

            ImGui::EndChild();
        }
    }

    ImGui::End();
}

void ImGUILayer::DrawNewMapPopup()
{
    if (m_bNewMapPopup)
    {
        ImGui::OpenPopup("Potential Unsaved Changes");
    }

    if (ImGui::BeginPopupModal("Potential Unsaved Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to create a new map?");

        constexpr ImVec4 textWarningCol = { 255, 0, 0, 255 };
        ImGui::TextColored(textWarningCol, "Any unsaved changes will be gone forever.");

        constexpr ImVec2 buttonSize = { 60.0f, 20.0f };
        constexpr float offsetFromMiddle = 20.0f;
        const float windowXHalfExtends = ImGui::GetWindowWidth() * 0.5f;

        ImGui::Spacing();

        ImGui::SetCursorPosX((windowXHalfExtends + offsetFromMiddle));
        if (ImGui::Button("Yes", buttonSize))
        {
            m_bNewMapPopup = false;
            g_globals.WorldEditor->CleanMap();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::SetCursorPosX(((windowXHalfExtends - offsetFromMiddle)) - buttonSize.x);
        if (ImGui::Button("No", buttonSize))
        {
            m_bNewMapPopup = false;
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::EndPopup();
}
