#include "precomp.h"

#include "Layers/ImGUI/ImGUILayer.h"

#include "FileHandler/FileHandler.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

#include "Config/Config.h"

#include "Shared/Game/SpriteTypes.hpp"

#include <Renderer/Renderer.h>

const bool ImGUILayer::HandleEvent(const SDL_Event& _event)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_ProcessEvent(&_event);

    return io.WantCaptureMouse;
}

ImGUILayer::ImGUILayer()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Border]              = { 0.24f, 0.24f, 0.24f, 1.0f };
    style.Colors[ImGuiCol_WindowBg]            = { 0.10f, 0.10f, 0.10f, 1.0f };
    style.Colors[ImGuiCol_TitleBg]             = { 0.14f, 0.14f, 0.14f, 1.0f };
    style.Colors[ImGuiCol_TitleBgActive]       = { 0.14f, 0.14f, 0.14f, 1.0f };
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
}

ImGUILayer::~ImGUILayer()
{
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

void ImGUILayer::DrawImGUI()
{
    if (ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode))
    {
 
        DrawMenuBar();
        DrawContentBrowser();
        DrawTileWindow();
    }
}

void ImGUILayer::DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", "Ctrl+E"))
        {
            ImGui::MenuItem("(file)", NULL, false, false);

            if (ImGui::MenuItem("Load", "CTRL+L")) std::string test = FileHandler::BrowseFile("C:/");
            if (ImGui::MenuItem("Save", "CTRL+S")) FileHandler::ExportLevelToJSON();

            ImGui::Separator();

            ImGui::MenuItem("(assets)", NULL, false, false);
            if (ImGui::MenuItem("Load Asset Folder", "CTRL+L")) std::string test = FileHandler::BrowseFile("C:/");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::MenuItem("Reset", "CTRL+R")) g_globals.Camera->Reset();
            if (ImGui::MenuItem("Goto Chunk", "CTRL+C+T")) 
            {
                ImGui::OpenPopup("Chunk");
            }

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

    if (ImGui::Begin("Content Browser", 0, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SeparatorText("Select Scenic Tile Texture:");
        ImGui::Spacing();

        for(U16 i = 0; i < Graphics::SPRITE_COUNT; i++) 
        {
            Opt<Sprite> optSprite = g_globals.Renderer->GetSprite(static_cast<Graphics::SpriteType>(i));

            if(optSprite.has_value())
            {
                Sprite& sprite = optSprite.value();

                ImGui::SameLine();

                const ImVec2 buttonSize = { 42.0f, 42.0f };
                if (ImGui::ImageButton(sprite.Texture, buttonSize))
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
        ImGui::Checkbox("Is Walkable:", &TileConfiguration.bIsWalkable);
        ImGui::Spacing();

        if (ImGui::InputInt("Z-Layer:", &TileConfiguration.RenderOrder))
        {
            TileConfiguration.RenderOrder = std::clamp<U32>(TileConfiguration.RenderOrder, 0, 255);
        }

        ImGui::SeparatorText("Preview Window:");

        ImVec2 windowSize  = ImGui::GetWindowSize();
        ImVec2 previewSize = { 84.0f, 84.0f };

        ImGui::SameLine((windowSize.x * 0.5f) - previewSize.x * 0.5f);
        
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        const float verticalOffset = contentRegionAvailable.y * 0.5f - 42.0f; 
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

        Opt<Sprite> optSprite = g_globals.Renderer->GetSprite(TileConfiguration.SpriteType);
        ImGui::Image(optSprite.value().Texture, { 84.0f, 84.0f });
    }
    ImGui::End();

    if (ImGui::Begin("Tool Settings:", 0, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Spacing();
        ImGui::Checkbox("Show Walkable Tiles:", &SettingsConfiguration.bShowWalkableTiles);

        ImGui::Spacing();
        ImGui::Checkbox("Show Chunk Outlines:", &SettingsConfiguration.bRenderChunkOutlines);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::SeparatorText("Transformation:");

        ImGui::Spacing();
        ImGui::Checkbox("Fill chunks:", &SettingsConfiguration.bFillChunks);

        if (!SettingsConfiguration.bFillChunks)
        {
            ImGui::Spacing();
            ImGui::SetNextItemWidth(100);
            ImGui::SliderInt("Tile Brush Size X:", &SettingsConfiguration.BrushSizeX, 1, 40);

            ImGui::Spacing();
            ImGui::SetNextItemWidth(100);
            ImGui::SliderInt("Tile Brush Size Y:", &SettingsConfiguration.BrushSizeY, 1, 40);
        }
        else 
        {
            ImGui::Text("Uncheck fill chunks for customization.");
        }
    }

    ImGui::End();
}
