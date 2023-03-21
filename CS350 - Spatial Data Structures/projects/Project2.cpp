/************************************************************************************//*!
\file           Project2.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of the implementation of Project2.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "Project2.h"

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions                                                                 */
/*-------------------------------------------------------------------------------------*/

void Project2::Initialise()
{
    CC::Keyboard::SetIsKeyRepeatable(false);

    #ifdef _DEBUG
        CC::LogTitleBlock("Initialising DX Engine");
        window = new CC::Window{ 1280, 720, "Project 2" };
    #else
        window = new CC::Window{ 1600, 900, "Project2" };
    #endif

    engine = &CC::Graphics::CreateEngine(window);

    

    editor = new CC::Editor{ *window, engine->GetDevice() };

    engine->GetDefaultView().Camera->SetPosition(CC::Vec3{ 0.0f, 0.0f, -5.0f });

    CC::Scene::SpatialPartitions sceneFlags = CC::Scene::SpatialPartitions::AABBTree;
    sceneFlags |= CC::Scene::SpatialPartitions::RitterSphereTree;
    sceneFlags |= CC::Scene::SpatialPartitions::LarssonSphereTree;
    sceneFlags |= CC::Scene::SpatialPartitions::PCASphereTree;
    scene = new CC::Scene{ "Project 2 Scene", sceneFlags };

    setupResources();
    setupScene();

    engine->SetCurrentScene(*scene);

    // Scene specific settings
    scene->RebuildLarssonTree(CC::Geometry::Sphere::Method::Larsson_6);
    selected = scene->GetDrawable("Bunny");
}

void Project2::Update()
{
    while (window->Update())
    {
        CC::FrameRateController::StartFrame();
        {
            if (CC::Keyboard::IsKeyPressed(CC::Keyboard::Keys::Escape))
                break;

            CC::Camera* defaultCam = engine->GetCamera("DefaultCam");
            moveMainCamera(*defaultCam);

            auto* defaultLight = scene->GetLight("DefaultLight");
            moveLight(*defaultLight);

            editor->Update();
            drawEditor();
            
            engine->Render(*editor, views);
            
        }
        CC::FrameRateController::EndFrame();
    }
}

void Project2::Exit()
{
    delete editor;

    CC::Graphics::DestroyEngine();
    delete window;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Member Definitions                                                 */
/*-------------------------------------------------------------------------------------*/

void Project2::setupResources()
{
    #ifdef _DEBUG
        CC::LogTitleBlock("Settup Up Models");
    #endif

    const std::string MODEL_PATH = "assets/Models/";

    const std::pair<std::string, std::string> models[] =
    {
        { "Bunny",          "bunny.obj"                 },
        { "Cup",            "cup.obj"                   },
        { "StarDestroyer",  "starwars1.obj"             },
        { "LucyPrinceton",  "lucy_princeton.obj"        },
        { "4Sphere",        "4Sphere.obj"               },
        { "Head",           "head.obj"                  },
        { "Teapot",         "teapot.obj"                },
        { "Suzanne",        "suzanne.obj"               },
        { "Skull",          "Skull_textured.fbx"        },
        { "Car",            "_2_Vintage_Car_01_low.fbx" }
    };

    for (unsigned int i = 0; i <ARRAYSIZE(models); ++i)
    {
        engine->LoadModel(models[i].first, MODEL_PATH + models[i].second);

        #ifdef _DEBUG
            CC::LogSeparator();
        #endif
    }
}

void Project2::setupScene()
{
    // Add a light
    CC::DirectionalLight directionalLight;
    /// White Light
    directionalLight.AmbientColour      = CC::Colour{ CC::Colours::White };
    directionalLight.AmbientIntensity   = 0.2f;
    directionalLight.DiffuseColour      = CC::Colour{ 0.8f, 0.8f, 0.8f, 1.0f };
    directionalLight.Direction          = CC::Vec3::UnitX;

    scene->AddDirectionalLight("DefaultLight", directionalLight);

    /// Red Light
    directionalLight.AmbientColour      = CC::Colour{ CC::Colours::Red };
    directionalLight.AmbientIntensity   = 0.5f;
    directionalLight.Direction          = CC::Vec3::UnitY;

    scene->AddDirectionalLight("RedLight", directionalLight);

    // Transforms
    const CC::Transform TRANSFORMS[] =
    {
                        // Position                     // Rotation                                     // Scale
        CC::Transform { CC::Vec3{ -5.5f, 4.5f, -5.0f }, CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 5.0f }            },  // Bunny
        CC::Transform { CC::Vec3{ -12.0f, 3.0f, 0.0f }, CC::Vec3{ 0.0f, 0.0f, -DirectX::XM_PIDIV4 },    CC::Vec3{ 3.0f }            },  // Teapot
        CC::Transform { CC::Vec3{ -8.0f, 0.0f, 0.0f },  CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 15.0f }           },  // Cup
        CC::Transform { CC::Vec3{ -6.0f, 1.0f, 2.0f },  CC::Vec3{ -DirectX::XM_PIDIV2, 0.0f, 0.0f },    CC::Vec3{ 15.0f }           },  // Cup2
        CC::Transform { CC::Vec3{ 3.0f, 1.5f, 0.0f },   CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 0.015f }          },  // Skull
        CC::Transform { CC::Vec3{ 5.0f, 5.5f, 1.0f },   CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 1.0f }            },  // Head
        CC::Transform { CC::Vec3{ 7.0f, 3.5f, 2.0f },   CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ 2.0f }            },  // Suzanne
        CC::Transform { CC::Vec3{ 3.0f, 5.5f, 4.0f },   CC::Vec3{ 0.4f, DirectX::XM_PIDIV2, -0.3f },    CC::Vec3{ 0.025f }          },  // StarDestroyer
        CC::Transform { CC::Vec3{ -5.0f, 5.0f, 5.0f },  CC::Vec3{ 0.0f, 0.0f, 0.0f },                   CC::Vec3{ 0.01f }           },  // LucyPrinceton
        CC::Transform { CC::Vec3{ -5.5f, 3.5f, -5.0f }, CC::Vec3{ 1.84f, 5.37f, 0.81f },                CC::Vec3{ 0.01f }           },  // 4Sphere
        CC::Transform { CC::Vec3{ 0.0f, 0.0f, 7.0f },   CC::Vec3{ CC::Vec3::Zero },                     CC::Vec3{ CC::Vec3::One }   },  // Car
    };

    scene->AddDrawable("Bunny",          CC::Drawable{ *engine->GetModel("Bunny"),           engine->GetSolidMaterial(), TRANSFORMS[0] });
    scene->AddDrawable("Teapot",         CC::Drawable{ *engine->GetModel("Teapot"),          engine->GetSolidMaterial(), TRANSFORMS[1] });
    scene->AddDrawable("Cup",            CC::Drawable{ *engine->GetModel("Cup"),             engine->GetSolidMaterial(), TRANSFORMS[2] });
    scene->AddDrawable("Cup2",           CC::Drawable{ *engine->GetModel("Cup"),             engine->GetSolidMaterial(), TRANSFORMS[3] });
    scene->AddDrawable("Skull",          CC::Drawable{ *engine->GetModel("Skull"),           engine->GetSolidMaterial(), TRANSFORMS[4] });
    scene->AddDrawable("Head",           CC::Drawable{ *engine->GetModel("Head"),            engine->GetSolidMaterial(), TRANSFORMS[5] });
    scene->AddDrawable("Suzanne",        CC::Drawable{ *engine->GetModel("Suzanne"),         engine->GetSolidMaterial(), TRANSFORMS[6] });
    scene->AddDrawable("StarDestroyer",  CC::Drawable{ *engine->GetModel("StarDestroyer"),   engine->GetSolidMaterial(), TRANSFORMS[7] });
    scene->AddDrawable("LucyPrinceton",  CC::Drawable{ *engine->GetModel("LucyPrinceton"),   engine->GetSolidMaterial(), TRANSFORMS[8] });
    scene->AddDrawable("4Sphere",        CC::Drawable{ *engine->GetModel("4Sphere"),         engine->GetSolidMaterial(), TRANSFORMS[9] });
    scene->AddDrawable("Car",            CC::Drawable{ *engine->GetModel("Car"),             engine->GetSolidMaterial(), TRANSFORMS[10] });
}

void Project2::drawEditor()
{
    if (editor->BeginWindow("CS350 Project 2"))
    {
        if (editor->BeginTabBar("Project 2 Tabs"))
        {
            // Bounding Volume Tab
            if (editor->BeginTab("BVs"))
            {
                editor->PushID("DrawBVs");
                {
                    const std::vector<std::string> LARSSON_METHODS{"EPOS-6", "EPOS-14", "EPOS-26", "EPOS-98"};
                    if (editor->EnumCombo("Larsson EPOS", larssonMethod, LARSSON_METHODS))
                    {
                        CC::Geometry::Sphere::Method epos = CC::Geometry::Sphere::Method::Larsson_6;
                        switch (larssonMethod)
                        {
                            case 1:     epos = CC::Geometry::Sphere::Method::Larsson_14;    break;
                            case 2:     epos = CC::Geometry::Sphere::Method::Larsson_26;    break;
                            case 3:     epos = CC::Geometry::Sphere::Method::Larsson_98;    break;
                            default:    epos = CC::Geometry::Sphere::Method::Larsson_6;     break;
                        }

                        //scene.RebuildLarssonTree(epos);
                    }

                    editor->Seperator();
                    editor->Text("Debug Draw");

                    bool drawAABB = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::AABB);
                    if (editor->CheckBox("Draw AABBs", drawAABB))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::AABB, drawAABB);
                    }

                    bool drawRitter = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::RitterSphere);
                    if (editor->CheckBox("Draw Ritter Spheres", drawRitter))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::RitterSphere, drawRitter);
                    }

                    bool drawLarsson = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::LarssonSphere);
                    if (editor->CheckBox("Draw Larsson Spheres", drawLarsson))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::LarssonSphere, drawLarsson);
                    }

                    bool drawPCA = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::PCASphere);
                    if (editor->CheckBox("Draw PCA Spheres", drawPCA))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::PCASphere, drawPCA);
                    }
                }
                editor->PopID();

                editor->EndTab();
            }

            // BVH Tab
            if (editor->BeginTab("BVHs"))
            {
                editor->PushID("DrawBVHs");
                {
                    const std::vector<std::string> TREE_METHODS{ "Top-Down", "Bottom-Up" };
                    if (editor->EnumCombo("Build Strategy", treeMethod, TREE_METHODS))
                    {
                        const CC::Geometry::AABBTree::Method STRATEGY = treeMethod == 0 ? CC::Geometry::AABBTree::Method::TopDown :
                                                                                          CC::Geometry::AABBTree::Method::BottomUp;
                        scene->SetTreeMethodAndBuild(STRATEGY);
                    }

                    editor->Seperator();
                    editor->Text("Debug Draw");

                    bool drawAABBTree = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::AABBTree);
                    if (editor->CheckBox("Draw AABBTree", drawAABBTree))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::AABBTree, drawAABBTree);
                    }

                    bool drawRitterTree = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::RitterSphereTree);
                    if (editor->CheckBox("Draw RitterSphereTree", drawRitterTree))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::RitterSphereTree, drawRitterTree);
                    }

                    bool drawLarssonTree = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::LarssonSphereTree);
                    if (editor->CheckBox("Draw LarssonSphereTree", drawLarssonTree))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::LarssonSphereTree, drawLarssonTree);
                    }

                    bool drawPCATree = engine->GetRenderer().GetDebugDrawState(CC::Renderer::DebugDraw::PCASphereTree);
                    if (editor->CheckBox("Draw PCASphereTree", drawPCATree))
                    {
                        engine->GetRenderer().SetDebugDrawState(CC::Renderer::DebugDraw::PCASphereTree, drawPCATree);
                    }

                    editor->Seperator();
                    editor->Text("Heights");

                    int aabbTreeHeight = scene->GetAABBTree()->GetHeight();
                    if (editor->InputBoxInt("AABBTree Height", aabbTreeHeight)) {};

                    int ritterTreeHeight = scene->GetRitterTree()->GetHeight();
                    if (editor->InputBoxInt("RitterSphereTree Height", ritterTreeHeight)) {};

                    int larssonTreeHeight = scene->GetLarssonTree()->GetHeight();
                    if (editor->InputBoxInt("LarssonTree Height", larssonTreeHeight)) {};

                    int pcaTreeHeight = scene->GetPCATree()->GetHeight();
                    if (editor->InputBoxInt("PCASphereTree Height", pcaTreeHeight)) {};
                }
                editor->PopID();

                editor->EndTab();
            }

            // Scene Tab
            if (editor->BeginTab("Scene"))
            {
                editor->PushID("Transforms");
                {
                    const std::vector<std::string> DRAWABLES
                    {
                        "Bunny",
                        "Teapot",
                        "Cup",
                        "Cup2",
                        "Skull",
                        "Head",
                        "Suzanne",
                        "StarDestroyer",
                        "LucyPrinceton",
                        "4Sphere",
                        "Car",
                    };

                    if (editor->EnumCombo("Drawable", selectedIndex, DRAWABLES))
                    {
                        selected = scene->GetDrawable(DRAWABLES[selectedIndex]);
                    }

                    CC::Vec3 pos = selected->GetTransform().GetPosition();
                    if (editor->InputBoxFloat3("Position", pos))
                    {
                        selected->SetPosition(pos);
                    }

                    CC::Vec3 rot = selected->GetTransform().GetRotation();
                    if (editor->InputBoxFloat3("Rotation", rot))
                    {
                        selected->SetRotation(rot);
                    }

                    CC::Vec3 scale = selected->GetTransform().GetScale();
                    if (editor->InputBoxFloat3("Scale", scale))
                    {
                        selected->SetScale(scale);
                    }

                    editor->Seperator();

                    if (editor->Button("Rebuild Scene"))
                    {
                        //scene.Build();
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

void Project2::moveMainCamera(CC::Camera& cam)
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

void Project2::moveLight(ClamChowder::Light& light)
{
    lightRotation -= CC::Math::Radians(1.0f);
    lightRotation = CC::Math::Wrap(lightRotation, 0.0f, 360.f);
    light.GetLight()->Direction = CC::Vec3::Transform(CC::Vec3::UnitX, CC::Mat4::CreateRotationY(lightRotation));
}
