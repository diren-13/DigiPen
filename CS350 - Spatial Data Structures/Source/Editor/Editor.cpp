/************************************************************************************//*!
\file           Editor.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 30, 2022
\brief          Contains the implementation for the Editor object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Headers
#include <pch.h>
// Primary Header
#include "Editor/Editor.h"
// External Headers
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
// Project Headers
#include "Tools/Console.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Editor::Editor(const Window& window, Device& device)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplWin32_Init(window.GetHandle());

        ImGui_ImplDX11_Init(device.GetHandle().Get(), device.GetContext().Get());

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsResizeFromEdges = true;

        // Set style
        ImGui::GetStyle().FrameRounding = 4.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;
    
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    Editor::~Editor()
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Editor::Update()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::GetCurrentContext()->NavMoveFlags = ImGuiNavMoveFlags_None;
    }

    void Editor::Render()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImGui::EndFrame();
    }

    void Editor::PushID(const std::string& ID)
    {
        ImGui::PushID(ID.c_str());
    }

    void Editor::PushID(int ID)
    {
        ImGui::PushID(ID);
    }

    void Editor::PopID()
    {
        ImGui::PopID();
    }

    bool Editor::BeginWindow(const std::string& title) const
    {
        return ImGui::Begin(title.c_str(), nullptr, defaultWindowFlags);
    }

    bool Editor::BeginTabBar(const std::string& title) const
    {
        return ImGui::BeginTabBar(title.c_str());
    }

    bool Editor::BeginTab(const std::string& title) const
    {
        return ImGui::BeginTabItem(title.c_str());
    }

    void Editor::EndWindow() const
    {
        ImGui::End();
    }

    void Editor::EndTabBar() const
    {
        ImGui::EndTabBar();
    }

    void Editor::EndTab() const
    {
        ImGui::EndTabItem();
    }

    void Editor::Seperator() const
    {
        ImGui::Separator();
    }

    void  Editor::Text(const std::string& title) const
    {
        ImGui::Text(title.c_str());
    }

    bool Editor::Button(const std::string& title) const
    {
        return ImGui::Button(title.c_str());
    }

    bool Editor::InputBoxInt(const std::string& title, int& value, float speed)
    {
        return ImGui::DragInt(title.c_str(), &value, speed);
    }

    bool Editor::InputBoxFloat(const std::string& title, float& value, float speed)
    {
        return ImGui::DragFloat(title.c_str(), &value, speed);
    }

    bool Editor::InputBoxFloat3(const std::string& title, Vec3& value, float speed)
    {
        float values[3]{ value.x, value.y, value.z };
        if (ImGui::DragFloat3(title.c_str(), values, speed))
        {
            value.x = values[0];
            value.y = values[1];
            value.z = values[2];

            return true;
        }
        return false;
    }

    bool Editor::SliderBoxInt(const std::string& title, int& value, int min, int max)
    {
        int tmp = value;
        if (ImGui::SliderInt(title.c_str(), &tmp, min, max))
        {
            value = tmp;
            return true;
        }
        return false;
    }


    bool Editor::CheckBox(const std::string& title, bool& value)
    {
        return ImGui::Checkbox(title.c_str(), &value);
    }

    bool Editor::EnumCombo(const std::string& title, int& value, const std::vector<std::string>& enumNames)
    {
        const std::string& INITIAL_NAME = value >= static_cast<int>(enumNames.size()) ? "Unknown" : enumNames[value];
        bool modified = false;
        if (ImGui::BeginCombo(title.c_str(), INITIAL_NAME.c_str(), ImGuiComboFlags_None))
        {
            for (int i = 0; i < enumNames.size(); ++i)
            {
                const bool IS_SELECTED = (value == i);
                if (ImGui::Selectable(enumNames[i].c_str(), IS_SELECTED))
                {
                    value = i;
                    modified = true;
                }
                if (IS_SELECTED)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        return modified;
    }




}