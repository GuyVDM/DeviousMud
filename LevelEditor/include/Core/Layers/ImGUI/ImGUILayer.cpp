#include "precomp.h"

#include "Core/Camera/Camera.h"
#include "Core/Config/Config.h"
#include "Core/FileHandler/FileHandler.h"
#include "Core/Globals/Globals.h"
#include "Core/Layers/ImGUI/ImGUILayer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/WorldEditor/WorldEditor.h"

#include "Shared/Game/SpriteTypes.hpp"

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
        io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        ImGui_ImplSDL2_InitForSDLRenderer(_window, _renderer);
        ImGui_ImplSDLRenderer2_Init(_renderer);
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Border]              = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_WindowBg]            = { 0.10f, 0.10f, 0.10f, 1.0f };
    style.Colors[ImGuiCol_TitleBg]             = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_TitleBgActive]       = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_TitleBgCollapsed]    = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_FrameBg]             = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_FrameBgHovered]      = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_FrameBgActive]       = { 0.14f, 0.14f, 0.14f, 1.0f };
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

        DEVIOUS_EVENT("Succesfully loaded icon: " << path);

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
        DrawViewPortHelperButtons();
        DrawContentBrowser();
        DrawTileWindow();
        DrawRightClickMenu();
    }
}

void ImGUILayer::DrawViewPortHelperButtons()
{
    using namespace App::Config;

    auto ImGuiDrawButton = [](const char* _label, const ImVec2& _parentPos, const float _yOffset, SDL_Texture* _tex, const bool bIsSelected = false)
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

    constexpr ImVec2 windowSize = { 64.0f, 320.0f };

    constexpr U32 viewportFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    ImGui::SetNextWindowSize(windowSize);
    
    ImGui::Begin("##Tools", nullptr, viewportFlags);
    {
        const ImVec2 menuPos = ImGui::GetItemRectMin();

        if (ImGuiDrawButton("##Single", menuPos, 30.0f, m_Icons[e_ImGuiIconType::ICON_BRUSH], 
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_BRUSH))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_BRUSH;
        }

        if(ImGuiDrawButton("##Fill", menuPos, 65.0f, m_Icons[e_ImGuiIconType::ICON_FILL],
           TileConfiguration.InteractionMode == e_InteractionMode::MODE_FILL))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_FILL;
        } 

        if (ImGuiDrawButton("##Path", menuPos, 100.0f, m_Icons[e_ImGuiIconType::ICON_PATH], 
            SettingsConfiguration.bShowWalkableTiles))
        {
            SettingsConfiguration.bShowWalkableTiles = !SettingsConfiguration.bShowWalkableTiles;
        }

        if (ImGuiDrawButton("##Reset", menuPos, 135.0f, m_Icons[e_ImGuiIconType::ICON_RESET]))
        {
            g_globals.Camera->Reset();
        }

        if (ImGuiDrawButton("##ChunkGrid", menuPos, 170.0f, m_Icons[e_ImGuiIconType::ICON_GRID],
            SettingsConfiguration.bRenderChunkOutlines))
        {
            SettingsConfiguration.bRenderChunkOutlines = !SettingsConfiguration.bRenderChunkOutlines;
        }

        if (ImGuiDrawButton("##Picker", menuPos, 205.0f, m_Icons[e_ImGuiIconType::ICON_PICKER],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_PICKER))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_PICKER;
        }

        if (ImGuiDrawButton("##Select", menuPos, 240.0f, m_Icons[e_ImGuiIconType::ICON_SELECT],
            TileConfiguration.InteractionMode == e_InteractionMode::MODE_SELECTION))
        {
            TileConfiguration.InteractionMode = e_InteractionMode::MODE_SELECTION;
        }

        if (ImGuiDrawButton("##Wand", menuPos, 275.0f, m_Icons[e_ImGuiIconType::ICON_WAND],
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
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", "Ctrl+E"))
        {
            ImGui::MenuItem("(file)", NULL, false, false);

            if (ImGui::MenuItem("Load Map", "CTRL+L")) std::string test = FileHandler::BrowseFile("C:/");
            if (ImGui::MenuItem("Save Map", "CTRL+S")) FileHandler::ExportLevelToJSON();

            ImGui::Separator();

            ImGui::MenuItem("(assets)", NULL, false, false);
            if (ImGui::MenuItem("Load Asset Folder", "CTRL+L")) std::string test = FileHandler::BrowseFile("C:/");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::MenuItem("Reset", "CTRL+R")) g_globals.Camera->Reset();

            ImGui::EndMenu();
        }

        //if (ImGui::BeginPopupModal("Chunk", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        //{
        //    static U32 chunkId[2] = { 0, 0 };
        //    ImGui::Text("Fill in the target chunk coordinates.");

        //    ImGui::Separator();

        //    ImGui::InputInt2("Chunkcoords:", chunkId);
        //    if (ImGui::Button("Go to:", ImVec2(42.0f, 42.0f)))
        //    {
        //        g_globals.Camera->Position =
        //        {
        //            chunkId[0] * App::Config::GRIDSIZE,
        //            chunkId[1] * App::Config::GRIDSIZE
        //        };
        //    }

        //    if (ImGui::Button("Close"))
        //    {
        //        ImGui::CloseCurrentPopup();
        //    }

        //    ImGui::EndPopup();
        //}
    }
    ImGui::EndMainMenuBar();
}

void ImGUILayer::DrawContentBrowser()
{
    using namespace App::Config;

    if (TileConfiguration.CurrentTileType != e_EntityType::ENTITY_SCENIC)
        return;

    if (ImGui::Begin("Content Browser", 0, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SeparatorText("Select Tile Texture:");
        ImGui::Spacing();

        for(U16 i = 0; i < Graphics::SPRITE_COUNT; i++) 
        {
            Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(static_cast<Graphics::SpriteType>(i));

            if(optSprite.has_value())
            {
                Sprite& sprite = optSprite.value();

                ImGui::SameLine();

                constexpr ImVec2 uv0 = ImVec2(0.0f, 0.0f);
                const     ImVec2 uv1 = ImVec2(1.0f / static_cast<float>(sprite.FrameCount), 1.0f);

                const ImVec2 buttonSize = { 42.0f, 42.0f };
                if (ImGui::ImageButton(sprite.Texture, buttonSize, uv0, uv1))
                {
                    TileConfiguration.SpriteType = static_cast<Graphics::SpriteType>(i);
                }
            }
        }
    }

    ImGui::End();
}

void ImGUILayer::DrawTileWindow()
{
    using namespace App::Config;

    if (ImGui::Begin("Tile Settings:", 0, ImGuiWindowFlags_NoCollapse))
    {
        if (ImGui::BeginTabBar("##SelectItem"))
        {
            if(ImGui::BeginTabItem("Tile"))
            {
                TileConfiguration.CurrentTileType = e_EntityType::ENTITY_SCENIC;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Npc"))
            {
                TileConfiguration.CurrentTileType = e_EntityType::ENTITY_NPC;
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        switch(TileConfiguration.CurrentTileType) 
        {
            case e_EntityType::ENTITY_SCENIC:
            {
                DrawScenicSettings();
            }
            break;

            case e_EntityType::ENTITY_NPC:
            {
                DrawNPCSettings();
            }
            break;
        }
    }
    ImGui::End();

    if (ImGui::Begin("Editor Settings:", 0, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SeparatorText("Transformation:");

        if (TileConfiguration.InteractionMode == e_InteractionMode::MODE_BRUSH)
        {
            ImGui::Spacing();
            ImGui::SetNextItemWidth(100);
            ImGui::SliderInt2("Brush Size:", &SettingsConfiguration.BrushSize.x, 1, 16);
        }
        else 
        {
            ImGui::Text("Uncheck fill chunks for customization.");
        }
    }

    ImGui::End();
}

void ImGUILayer::DrawRightClickMenu() const
{
    using namespace App::Config;

    static bool bIsOpened = false;
    static U32  framesOpened = 0;

    if (!bIsOpened)
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

            ImGui::Separator();
            if (ImGui::MenuItem("Reset Camera")) g_globals.Camera->Reset();
 
            ImGui::Separator();
            if (ImGui::MenuItem("Copy Chunk")) g_globals.WorldEditor->CopyChunk();

            if (!TileConfiguration.ChunkClipboard.expired())
            {
                if (ImGui::MenuItem("Paste Chunk")) g_globals.WorldEditor->PasteChunk();
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

    ImGui::SeparatorText("Tile Settings:");

    ImGui::Text("Is Walkable:");
    ImGui::Checkbox("##IsWalkable:", &TileConfiguration.bIsWalkable);
    ImGui::Spacing();

    ImGui::SeparatorText("Preview Window:");

    const     ImVec2 windowSize  = ImGui::GetWindowSize();
    constexpr ImVec2 previewSize = { 84.0f, 84.0f };

    ImGui::SameLine((windowSize.x * 0.5f) - previewSize.x * 0.5f);

    const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
    const float verticalOffset = contentRegionAvailable.y * 0.5f - 42.0f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

    Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(TileConfiguration.SpriteType);

    if (!optSprite.has_value())
    {
        return;
    }

    Sprite& sprite = optSprite.value();

    constexpr ImVec2 uv0 = ImVec2(0.0f, 0.0f);
    const     ImVec2 uv1 = ImVec2(1.0f / static_cast<float>(sprite.FrameCount), 1.0f);
    ImGui::Image(optSprite.value().Texture, { 84.0f, 84.0f }, uv0, uv1);
}

void ImGUILayer::DrawNPCSettings()
{
    using namespace App::Config;

    ImGui::Spacing();

    static I32 npcId = 1;
    ImGui::SeparatorText("NPC Settings:");
    ImGui::Text("Id:");
    if (ImGui::InputInt("##NPCId", &npcId)) 
    {
        npcId = std::clamp<U32>(npcId, 0, INT_FAST32_MAX);
        TileConfiguration.NPCDefinition = get_npc_definition(npcId);
    }

    ImGui::Spacing();

    static float respawnTime = 1.0f;
    ImGui::Text("Respawn Time:");
    ImGui::InputFloat("##RespawnTime", &respawnTime, 0.5f, 1.0f);
    ImGui::Spacing();

    ImGui::Spacing();

    ImGui::SeparatorText("Preview:");

    ImGui::Text("Name:");
    ImGui::SameLine();
    ImGui::TextColored({ 0, 255, 0, 255 }, TileConfiguration.NPCDefinition.name.c_str());

    ImGui::Text("Combat Level:");
    ImGui::SameLine();

    const std::string levelHeader = "Level-" + std::to_string(TileConfiguration.NPCDefinition.combatLevel);
    ImGui::TextColored({ 0, 255, 0, 255 }, levelHeader.c_str());

    ImGui::Text("Size:");
    ImGui::SameLine();
    ImGui::TextColored({ 0, 255, 0, 255 }, std::to_string(TileConfiguration.NPCDefinition.size).c_str());

    ImGui::Text("Sprite Id:");
    ImGui::SameLine();
    ImGui::TextColored({ 0, 255, 0, 255 }, std::to_string(static_cast<U32>(TileConfiguration.NPCDefinition.sprite)).c_str());

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

        Optional<Sprite> optSprite = g_globals.Renderer->GetSprite(TileConfiguration.NPCDefinition.sprite);

        if (!optSprite.has_value())
        {
            return;
        }

        Sprite& sprite = optSprite.value();

        constexpr ImVec2 uv0 = ImVec2(0.0f, 0.0f); 
        const     ImVec2 uv1 = ImVec2(1.0f / static_cast<float>(sprite.FrameCount), 1.0f);
        ImGui::Image(optSprite.value().Texture, { 84.0f, 84.0f }, uv0, uv1);

    }
}
