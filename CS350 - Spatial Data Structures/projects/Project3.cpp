/************************************************************************************//*!
\file           Project3.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of the implementation of Project3.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "Project3.h"

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions                                                                 */
/*-------------------------------------------------------------------------------------*/

void Project3::Initialise()
{
    CC::Keyboard::SetIsKeyRepeatable(false);

    #ifdef _DEBUG
        CC::LogTitleBlock("Initialising DX Engine");
        window = new CC::Window{ 1280, 720, "Project 3" };
    #else
        window = new CC::Window{ 1600, 900, "Project 3" };
    #endif

    engine = &CC::Graphics::CreateEngine(window);

    editor = new CC::Editor{ *window, engine->GetDevice() };

    setupResources();
    setupScenes();

    engine->GetDefaultView().Camera->SetPosition(CC::Vec3{ 0.0f, 0.0f, -5.0f });
    engine->SetCurrentScene(scenes.front());
}

void Project3::Update()
{
    while (window->Update())
    {
        CC::FrameRateController::StartFrame();
        {
            if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::Escape))
                break;

            CC::Camera* defaultCam = engine->GetCamera("DefaultCam");
            moveMainCamera(*defaultCam);

            auto* defaultLight = engine->GetCurrentScene().GetLight("DefaultLight");
            moveLight(*defaultLight);

            editor->Update();
            drawEditor();
            
            engine->Render(*editor, views);
            
        }
        CC::FrameRateController::EndFrame();
    }
}

void Project3::Exit()
{
    delete editor;

    CC::Graphics::DestroyEngine();
    delete window;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Member Definitions                                                 */
/*-------------------------------------------------------------------------------------*/

void Project3::setupResources()
{
    #ifdef _DEBUG
        CC::LogTitleBlock("Setup Up Models");
    #endif

    const std::string MODEL_PATH = "assets/Models/";

    const std::pair<std::string, std::string> models[] =
    {
        { "Bunny",          "bunny.obj"                 },
        { "Teapot",         "teapot.obj"                },
        { "Cup",            "cup.obj"                   },
        { "Skull",          "Skull_textured.fbx"        },
        { "Head",           "head.obj"                  },
        { "Suzanne",        "suzanne.obj"               },
        { "StarDestroyer",  "starwars1.obj"             },
        { "LucyPrinceton",  "lucy_princeton.obj"        },
        { "4Sphere",        "4Sphere.obj"               },
    };

    for (unsigned int i = 0; i < ARRAYSIZE(models); ++i)
    {
        engine->LoadModel(models[i].first, MODEL_PATH + models[i].second);

        #ifdef _DEBUG
            CC::LogSeparator();
        #endif
    }
}

void Project3::setupScenes()
{
    // Add a light to every scene
    CC::DirectionalLight directionalLight;
    /// White Light
    directionalLight.AmbientColour      = CC::Colour{ CC::Colours::White };
    directionalLight.AmbientIntensity   = 0.2f;
    directionalLight.DiffuseColour      = CC::Colour{ 0.8f, 0.8f, 0.8f, 1.0f };
    directionalLight.Direction          = CC::Vec3::UnitX;

    // Transforms
    const CC::Transform TRANSFORMS[] =
    {
                        // Position                     // Rotation                                     // Scale
        CC::Transform { CC::Vec3{ 0.0f,  1.0f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 5.0f }            },   // Bunny1
        CC::Transform { CC::Vec3{-1.0f,  2.0f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ -5.0f, 5.0f, 5.0f }},  // Bunny2
        CC::Transform { CC::Vec3{-4.0f,  3.0f, 0.0f }, CC::Vec3{ 0.0f, 0.0f, -DirectX::XM_PIDIV4 },    CC::Vec3{ 3.0f }            },   // Teapot
        CC::Transform { CC::Vec3{ 0.0f,  0.0f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 15.0f }           },   // Cup
        CC::Transform { CC::Vec3{ 0.0f,  1.5f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 0.015f }          },   // Skull
        CC::Transform { CC::Vec3{ 0.0f,  5.5f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 1.0f }            },   // Head
        CC::Transform { CC::Vec3{ 0.0f,  3.5f, 0.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 2.0f }            },   // Suzanne
        CC::Transform { CC::Vec3{ 0.0f,  5.5f, 0.0f }, CC::Vec3{ 0.4f, DirectX::XM_PIDIV2, -0.3f },    CC::Vec3{ 0.025f }          },   // StarDestroyer
        CC::Transform { CC::Vec3{ 0.0f,  5.0f, 5.0f }, CC::Vec3{ 0.0f, 0.0f, 0.0f },                   CC::Vec3{ 0.01f }           },   // LucyPrinceton
        CC::Transform { CC::Vec3{ 0.0f,  3.5f, 0.0f }, CC::Vec3{ 1.84f, 5.37f, 0.81f },                CC::Vec3{ 0.01f }           },   // 4Sphere
    };

    CC::Scene::SpatialPartitions sceneFlags = CC::Scene::SpatialPartitions{0};
    sceneFlags |= CC::Scene::SpatialPartitions::Octree;
    sceneFlags |= CC::Scene::SpatialPartitions::BSPTree;    

    scenes.emplace_back(CC::Scene{ "Sphere",        sceneFlags });
    scenes.emplace_back(CC::Scene{ "Teapot & Cup",  sceneFlags });
    scenes.emplace_back(CC::Scene{ "Two Bunnies",   sceneFlags });
    scenes.emplace_back(CC::Scene{ "Skull",         sceneFlags });
    scenes.emplace_back(CC::Scene{ "Head",          sceneFlags });
    scenes.emplace_back(CC::Scene{ "Suzanne",       sceneFlags });
    scenes.emplace_back(CC::Scene{ "StarDestroyer", sceneFlags });
    scenes.emplace_back(CC::Scene{ "LucyPrinceton", sceneFlags });
    scenes.emplace_back(CC::Scene{ "4Sphere",       sceneFlags });

    for (size_t i = 0; i < scenes.size(); ++i)
    {
        scenes[i].AddDirectionalLight("DefaultLight", directionalLight);

        switch (i)
        {
            case 0:
            {
                scenes[i].AddDrawable("Sphere",         engine->GetSphere(),                                engine->GetSolidMaterial(), CC::Transform{ CC::Vec3::Zero, CC::Vec3::Zero, CC::Vec3 { 5.0f } });
                break;
            }
            case 1:
            {
                scenes[i].AddDrawable("Teapot",         CC::Drawable{*engine->GetModel("Teapot"),           engine->GetSolidMaterial(), TRANSFORMS[2]});
                scenes[i].AddDrawable("Cup",            CC::Drawable{*engine->GetModel("Cup"),              engine->GetSolidMaterial(), TRANSFORMS[3]});
                break;
            }
            case 2:
            {
                scenes[i].AddDrawable("Bunny1",         CC::Drawable{ *engine->GetModel("Bunny"),           engine->GetSolidMaterial(), TRANSFORMS[0] });
                scenes[i].AddDrawable("Bunny2",         CC::Drawable{*engine->GetModel("Bunny"),            engine->GetSolidMaterial(), TRANSFORMS[1]});
                break;
            }
            case 3:
            {
                scenes[i].AddDrawable("Skull",          CC::Drawable{ *engine->GetModel("Skull"),           engine->GetSolidMaterial(), TRANSFORMS[4] });
                break;
            }
            case 4:
            {
                scenes[i].AddDrawable("Head",           CC::Drawable{ *engine->GetModel("Head"),            engine->GetSolidMaterial(), TRANSFORMS[5] });
                break;
            }
            case 5:
            {
                scenes[i].AddDrawable("Suzanne",        CC::Drawable{ *engine->GetModel("Suzanne"),         engine->GetSolidMaterial(), TRANSFORMS[6] });
                break;
            }
            case 6:
            {
                scenes[i].AddDrawable("StarDestroyer",  CC::Drawable{ *engine->GetModel("StarDestroyer"),   engine->GetSolidMaterial(), TRANSFORMS[7] });
                break;
            }
            case 7:
            {
                scenes[i].AddDrawable("LucyPrinceton",  CC::Drawable{ *engine->GetModel("LucyPrinceton"),   engine->GetSolidMaterial(), TRANSFORMS[8] });
                break;
            }
            case 8:
            {
                scenes[i].AddDrawable("4Sphere",        CC::Drawable{ *engine->GetModel("4Sphere"),         engine->GetSolidMaterial(), TRANSFORMS[9] });
                break;
            }
            default: break;
        }
    }
}

void Project3::drawEditor()
{
    if (editor->BeginWindow("CS350 Project 3"))
    {
        if (editor->BeginTabBar("Project 3 Tabs"))
        {
            // Spatial Partition Tab
            if (editor->BeginTab("Spatial Partitions"))
            {
                editor->PushID("Draw Trees");
                {
                    bool drawOctree = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::Octree);
                    if (editor->CheckBox("Draw Octree", drawOctree))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::Octree, drawOctree);
                    }

                    bool drawOctreeData = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::OctreeData);
                    if (editor->CheckBox("Draw Octree Data", drawOctreeData))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::OctreeData, drawOctreeData);
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::BSPTreeData, false);
                    }

                    bool drawBSPTreeData = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::BSPTreeData);
                    if (editor->CheckBox("Draw BSPTree Data", drawBSPTreeData))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::BSPTreeData, drawBSPTreeData);
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::OctreeData, false);
                    }

                    editor->Seperator();
                    editor->Text("Heights");

                    int octreeHeight = engine->GetCurrentScene().GetOctree()->GetHeight();
                    if (editor->InputBoxInt("Octree Height", octreeHeight)) {};

                    int bspTreeHeight = engine->GetCurrentScene().GetBSPTree()->GetHeight();
                    if (editor->InputBoxInt("BSPTree Height", bspTreeHeight)) {};

                }
                editor->PopID();

                editor->EndTab();
            }

            // Scenes Tab
            if (editor->BeginTab("Scenes"))
            {
                editor->PushID("Scenes");
                {
                    const std::vector<std::string> SCENES
                    {
                        "Sphere",
                        "Teapot & Cup",
                        "Two Bunnies",
                        "Skull",
                        "Head",
                        "Suzanne",
                        "StarDestroyer",
                        "LucyPrinceton",
                        "4Sphere"
                    };

                    if (editor->EnumCombo("Scene", selectedIndex, SCENES))
                    {
                        engine->SetCurrentScene(scenes[selectedIndex]);
                    }

                    int octreeNodeThreshold = engine->GetCurrentScene().GetOctree()->GetNodeThreshold();
                    if (editor->SliderBoxInt("Octree Node Threshold", octreeNodeThreshold, CC::Geometry::Octree::MIN_THRESHOLD, CC::Geometry::Octree::MAX_THRESHOLD))
                    {
                        engine->GetCurrentScene().Octree()->SetNodeThreshold(octreeNodeThreshold);
                    }

                    int bspNodeThreshold = engine->GetCurrentScene().GetBSPTree()->GetNodeThreshold();
                    if (editor->SliderBoxInt("BSPTree Node Threshold", bspNodeThreshold, CC::Geometry::BSPTree::MIN_THRESHOLD, CC::Geometry::BSPTree::MAX_THRESHOLD))
                    {
                        engine->GetCurrentScene().BSPTree()->SetNodeThreshold(bspNodeThreshold);
                    }

                    editor->Seperator();

                    if (editor->Button("Rebuild Scene"))
                    {
                        engine->GetCurrentScene().Rebuild();
                    }
                }
                editor->PopID();

                editor->EndTab();
            }

            // Setting Tab
            if (editor->BeginTab("Settings"))
            {
                editor->PushID("Settings");
                {
                    bool drawDefaultGrid = engine->GetRenderDefaultGrid();
                    if (editor->CheckBox("Render Grid", drawDefaultGrid))
                    {
                        engine->SetRenderDefaultGrid(drawDefaultGrid);
                    }

                    bool drawDefaultAxes = engine->GetRenderDefaultAxes();
                    if (editor->CheckBox("Render Axes", drawDefaultAxes))
                    {
                        engine->SetRenderDefaultAxes(drawDefaultAxes);
                    }

                    editor->Seperator();
                    editor->Text("Minimap");

                    bool minimapActive = engine->GetMinimapView().active;
                    if (editor->CheckBox("Minimap On", minimapActive))
                    {
                        engine->GetMinimapView().active = minimapActive;
                    }

                    if (minimapActive)
                    {
                        editor->PushID("MinimapSetting");
                        {
                            bool drawMinimapAxes = engine->GetRenderMinimapAxes();
                            if (editor->CheckBox("Render Axes", drawMinimapAxes))
                            {
                                engine->SetRenderMinimapAxes(drawMinimapAxes);
                            }

                            bool drawMinimapFOV = engine->GetRenderMinimapFOV();
                            if (editor->CheckBox("Render FOV", drawMinimapFOV))
                            {
                                engine->SetRenderMinimapFOV(drawMinimapFOV);
                            }

                            float minimapZoom = engine->GetMinimapView().Camera->GetPosition().y;
                            if (editor->InputBoxFloat("Minimap Zoom", minimapZoom))
                            {
                                CC::Vec3 minimapCamPos = engine->GetMinimapView().Camera->GetPosition();
                                minimapCamPos.y = minimapZoom;
                                engine->GetMinimapView().Camera->SetPosition(minimapCamPos);
                            }
                        }
                        editor->PopID();
                    }
                }
                editor->PopID();

                editor->EndTab();
            }

            editor->EndTabBar();
        }
    }
    editor->EndWindow();
}

void Project3::moveMainCamera(CC::Camera& cam)
{
    static constexpr float CAM_SPEED = 0.075f;

    while (!CC::Mouse::GetEventBuffer().empty())
    {
        CC::MouseEvent mouseEvent = CC::Mouse::ReadEvent();
        if (CC::Mouse::GetIsRightDown())
        {
            if (mouseEvent.GetMouseState() == CC::MouseEvent::MouseState::RawMove)
            {
                const CC::MousePosition& MOUSE_POS = mouseEvent.GetMousePosition();
                const CC::Vec3 R{ static_cast<float>(MOUSE_POS.Y) * 0.0025f, static_cast<float>(MOUSE_POS.X) * 0.0025f, 0.0f };

                cam.RotateCamera(R);
            }
        }
    }

    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::W))
    {
        cam.MoveCamera(cam.GetForward() * CAM_SPEED);
    }
    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::D))
    {
        cam.MoveCamera(cam.GetRight() * CAM_SPEED);
    }
    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::S))
    {
        cam.MoveCamera(cam.GetBack() * CAM_SPEED);
    }
    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::A))
    {
        cam.MoveCamera(cam.GetLeft() * CAM_SPEED);
    }
    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::Space))
    {
        cam.MoveCamera(CC::Vec3{ 0.0f, CAM_SPEED, 0.0f });
    }
    if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::Z))
    {
        cam.MoveCamera(CC::Vec3{ 0.0f, -CAM_SPEED, 0.0f });
    }
}

void Project3::moveLight(ClamChowder::Light& light)
{
    lightRotation -= CC::Math::Radians(1.0f);
    lightRotation = CC::Math::Wrap(lightRotation, 0.0f, 360.f);
    light.GetLight()->Direction = CC::Vec3::Transform(CC::Vec3::UnitX, CC::Mat4::CreateRotationY(lightRotation));
}
