#include "precomp.h"

#include "Layers/ImGUI/ImGUILayer.h"

#include "FileHandler/FileHandler.h"

#include "Camera/Camera.h"

#include "Globals/Globals.h"

const bool ImGUILayer::HandleEvent(const SDL_Event& _event)
{
    return ImGui_ImplSDL2_ProcessEvent(&_event);
}

ImGUILayer::ImGUILayer()
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

ImGUILayer::~ImGUILayer()
{
}

void ImGUILayer::DrawImGUI()
{
    DrawMenuBar();
}

void ImGUILayer::DrawMenuBar()
{
    //ImGui::ShowDemoWindow();

    ImVec2 newPadding = ImVec2(10.0f, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, newPadding);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", "Ctrl+E"))
        {
            if (ImGui::MenuItem("Save", "CTRL+S")) FileHandler::ExportLevelToJSON();
            if (ImGui::MenuItem("Load", "CTRL+L")) 
            {
                std::string test = FileHandler::BrowseFile("C:/");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::MenuItem("Reset", "CTRL+R")) g_globals.Camera->Reset();
            ImGui::EndMenu();
        }
    }
    ImGui::PopStyleVar();
    ImGui::EndMainMenuBar();
}
