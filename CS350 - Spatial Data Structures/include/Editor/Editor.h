/************************************************************************************//*!
\file           Editor.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 30, 2022
\brief          Contains the interface for the Editor object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <string>
// External Headers
#include <ImGui/imgui.h>
// Project Headers
#include "Window/Window.h"
#include "Graphics/Device.h"
#include "Math/MathDefines.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class Editor
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Editor(const Window& window, Device& device);
        ~Editor();

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                    Update          ();
        static void             Render          ();

        static void             PushID          (const std::string& ID);
        static void             PushID          (int ID);
        static void             PopID           ();

        // UI
        [[nodiscard]] bool      BeginWindow     (const std::string& title) const;
        [[nodiscard]] bool      BeginTabBar     (const std::string& title) const;
        [[nodiscard]] bool      BeginTab        (const std::string& title) const;
        void                    EndWindow       () const;
        void                    EndTabBar       () const;
        void                    EndTab          () const;

        void                    Seperator       () const;
        void                    Text            (const std::string& title) const;
        [[nodiscard]] bool      Button          (const std::string& title) const;
        [[nodiscard]] bool      CheckBox        (const std::string& title, bool& value);
        [[nodiscard]] bool      InputBoxInt     (const std::string& title, int& value, float speed = 0.1f);
        [[nodiscard]] bool      InputBoxFloat   (const std::string& title, float& value, float speed = 0.1f);
        [[nodiscard]] bool      InputBoxFloat3  (const std::string& title, Vec3& value, float speed = 0.1f);
        [[nodiscard]] bool      SliderBoxInt    (const std::string& title, int& value, int min, int max);
        [[nodiscard]] bool      EnumCombo       (const std::string& title, int& value, const std::vector<std::string>& enumNames);

    private:
        ImGuiWindowFlags    defaultWindowFlags  = ImGuiWindowFlags_NoNav;
    };
}
