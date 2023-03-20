/*+======================================================================================
File:       main.cpp

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu
  
Summary:    Starting point of the application.
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Project Headers
#include "Camera.h"
#include "ModelLoader.h"

/*-------------------------------------------------------------------------------------*/
/* Defines                                                                             */
/*-------------------------------------------------------------------------------------*/
#define APPLICATION_NAME    "RTR"
#define ENGINE_NAME         "Vulkan Engine"
#define APPLICATION_NAME_W  L"RTR"
#define ENGINE_NAME_W       L"Vulkan Engine"

#define DEFAULT_VERT_SHADER "Assets/Shaders/defaultVert.spv"
#define DEFAULT_FRAG_SHADER "Assets/Shaders/defaultFrag.spv"


/*-------------------------------------------------------------------------------------*/
/* Type Definitions                                                                    */
/*-------------------------------------------------------------------------------------*/
struct PushConstant
{
    glm::mat4 LocalToCamera;
    glm::vec4 LightPos;             // Local space
    glm::vec4 CameraPos;            // Local space
    glm::vec4 AmbientLightColour;
    glm::vec4 LightColour;          
};


/*-------------------------------------------------------------------------------------*/
/* Main                                                                                */
/*-------------------------------------------------------------------------------------*/

using namespace RTR;

int WINAPI main
(
    HINSTANCE hInstance, 
    [[maybe_unused]]HINSTANCE hPrevInstance, 
    [[maybe_unused]]LPSTR lpCmdLine, 
    [[maybe_unused]]int nCmdShow
)
{

    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    (void)freopen("CON", "w", stdout);
    (void)freopen("CON", "w", stderr);
    SetConsoleTitle(L"RTR");

    try
    {
        RTR::Config config;
        config.enableDebugging  = true;
        config.enableRenderDoc  = true;

        RTR::Window window{ hInstance, APPLICATION_NAME_W };

        Instance vkInstance
        {
            config.enableDebugging,
            config.enableRenderDoc,
            InstanceProperties{ APPLICATION_NAME, ENGINE_NAME }
        };

        Canvas canvas { vkInstance, window };

        // Setup vertex descriptor
        VertexDescriptor::Attribute attributes[4] =
        {
            VertexDescriptor::Attribute
            {
                .format = VertexDescriptor::Format::Float_3D,
                .offset = 0
            },
            VertexDescriptor::Attribute
            {
                .format = VertexDescriptor::Format::Float_2D,
                .offset = offsetof(Geometry::Vertex, uv)
            },
            VertexDescriptor::Attribute
            {
                .format = VertexDescriptor::Format::Float_3D,
                .offset = offsetof(Geometry::Vertex, nml)
            },
            VertexDescriptor::Attribute
            {
                .format = VertexDescriptor::Format::Float_3D,
                .offset = offsetof(Geometry::Vertex, tan)
            }
        };

        VertexDescriptor vertexDescriptor { attributes, ARRAYSIZE(attributes), sizeof(Geometry::Vertex) }; 

        // Push Constants
        Pipeline::PushConstant pushConstant = 
        {
            .offset     = 0U,
            .size       = sizeof(PushConstant),
            .allStages  = true
        };

        // Set up pipeline
        GraphicsPipeline::Setup graphicsPipelineSetup;

        // Make shaders
        ShaderModule    vertexShader    = vkInstance.CreateShader(ShaderType::Vertex, DEFAULT_VERT_SHADER);
        ShaderModule    fragmentShader  = vkInstance.CreateShader(ShaderType::Fragment, DEFAULT_FRAG_SHADER);

        graphicsPipelineSetup.shaders.emplace_back(&vertexShader);
        graphicsPipelineSetup.shaders.emplace_back(&fragmentShader);

        // Decriptor Set for Textures
        std::vector imageSamplers =
        {
            Descriptor { Descriptor::Type::ImageSampler, Descriptor::ShaderStage::Fragment, 0 },    // DiffuseSampler
            Descriptor { Descriptor::Type::ImageSampler, Descriptor::ShaderStage::Fragment, 1 },    // AmbientOcclusionSampler
            Descriptor { Descriptor::Type::ImageSampler, Descriptor::ShaderStage::Fragment, 2 },    // NormalSampler
            Descriptor { Descriptor::Type::ImageSampler, Descriptor::ShaderStage::Fragment, 3 },    // RoughnessSampler
        };

        DescriptorSetLayout imageSamplerLayout(vkInstance.GetDevice(), imageSamplers);

        graphicsPipelineSetup.descriptorSetLayouts.emplace_back(&imageSamplerLayout);
        
        graphicsPipelineSetup.renderPass        = &canvas.GetRenderPass();
        graphicsPipelineSetup.pushConstant      = &pushConstant;
        graphicsPipelineSetup.vertexDescriptor  = &vertexDescriptor;

        GraphicsPipeline vulkanGraphicsPipeline{ vkInstance.GetDevice(), graphicsPipelineSetup };

        // Set up buffers
        //Buffer vertexBuffer{ vkInstance.GetDevice(), Buffer::Type::Vertex, sizeof(Geometry::Vertex), static_cast<uint32_t>(Geometry::Cube::VERTICES.size()) };
        //vertexBuffer.MapMemory(Geometry::Cube::VERTICES.data());
        //Buffer indexBuffer{ vkInstance.GetDevice(), Buffer::Type::Index, sizeof(Geometry::Cube::INDICES[0]), static_cast<uint32_t>(Geometry::Cube::INDICES.size()) };
        //indexBuffer.MapMemory(Geometry::Cube::INDICES.data());

        // Allocate descriptor sets
        auto descriptorSet = vkInstance.GetDevice().AllocateDescriptors(graphicsPipelineSetup.descriptorSetLayouts);

        // Load Textures & Texture Samplers and write to descriptor sets
        const std::vector<std::string> PATHS =
        {
            "Assets/Textures/Skull/TD_Checker_Base_Color.dds",
            "Assets/Textures/Skull/TD_Checker_Mixed_AO.dds",
            "Assets/Textures/Skull/TD_Checker_Normal_OpenGL.dds",
            "Assets/Textures/Skull/TD_Checker_Roughness.dds"
        };

        TextureSampler  diffuseSampler  { vkInstance.GetDevice() };
        TextureSampler  aoSampler       { vkInstance.GetDevice() };
        TextureSampler  normalSampler   { vkInstance.GetDevice() };
        TextureSampler  roughnessSampler{ vkInstance.GetDevice() };

        Texture         diffuse     { vkInstance.GetDevice(), PATHS[0] };
        Texture         ao          { vkInstance.GetDevice(), PATHS[1] };
        Texture         normal      { vkInstance.GetDevice(), PATHS[2] };
        Texture         roughness   { vkInstance.GetDevice(), PATHS[3] };

        // Bind Image to descriptor set
        descriptorSet[0].WriteImage(diffuse, diffuseSampler, 0);
        descriptorSet[0].WriteImage(ao, aoSampler, 1);
        descriptorSet[0].WriteImage(normal, normalSampler, 2);
        descriptorSet[0].WriteImage(roughness, roughnessSampler, 3);

        // Setup Model
        ModelLoader modelLoader{ vkInstance.GetDevice() };

        std::string modelPath = "Assets/Models/Skull_textured.fbx";
        modelLoader.Load(modelPath);
        modelLoader.MapMemory();


        // Set up scene
        Camera      camera;
        glm::vec3   lastLightPos{ 0.0f, 1.0f, 0.0f };
        bool        fixLight = false;

        float   distance    = 3.0f;
        float   fov         = glm::radians(60.f);

        glm::vec3 rotationAxis      { 0.0f, 1.0f, 0.0f };
        glm::vec3 target            { 0.0f, 0.0f, 0.0f };

        // Matrices
        glm::mat4 modelToWorld      { 1.0f };
        modelToWorld[0][0] = modelToWorld[1][1] = modelToWorld[2][2] = 0.01f;
        glm::mat4 renderingMatrix   { 1.0f };

        while (!window.Update())
        {
            if (window.GetIsMinimized())
                continue;

            if (GetKeyState(VK_RBUTTON) & 0x8000)
            {
                const int X_OFFSET = window.MousePrevXPos - window.MouseXPos;
                const int Y_OFFSET = window.MousePrevYPos - window.MouseYPos;

                if (std::abs(X_OFFSET) > 2 || std::abs(Y_OFFSET) > 2)
                {
                    camera.Angles().y -= X_OFFSET != 0 ? static_cast<float>(X_OFFSET) * 0.005f : 0.0f;
                    camera.Angles().x -= Y_OFFSET != 0 ? static_cast<float>(Y_OFFSET) * 0.005f : 0.0f;
                }
            }

            if (GetKeyState(VK_SPACE) & 0x8000)
            {
                fixLight = !fixLight;
            }

            if (GetKeyState(0x57) & 0x8000)
            {
                distance -= 0.1f;
            }

            if (GetKeyState(0x53) & 0x8000)
            {
                distance += 0.1f;
            }

            if (!canvas.StartRendering())
                continue;

            const float AR = static_cast<float>(window.GetWindowWidth()) / static_cast<float>(window.GetWindowHeight());

            camera.LookAt(target, distance, camera.GetAngles());

            
            if (!fixLight)
            {
                glm::vec3 lightPos = camera.GetPosition() + glm::vec3{ 0.0f, 1.0f, 0.0f };
                lastLightPos = lightPos;
            }

            glm::mat4& projection = camera.ComputePespectiveMatrix(fov, AR);

            {
                canvas.BindPipeline(vulkanGraphicsPipeline);

                for (auto& ds : descriptorSet)
                {
                    canvas.BindDescriptorSet(ds);
                }

                const glm::mat4 WORLD_TO_MODEL = glm::inverse(modelToWorld);

                PushConstant pc =
                {
                    .LocalToCamera      = projection * camera.GetViewMatrix() * modelToWorld,
                    .LightPos           = WORLD_TO_MODEL * glm::vec4(lastLightPos.x, lastLightPos.y, lastLightPos.z, 1.0f),
                    .CameraPos          = WORLD_TO_MODEL * glm::vec4(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f),
                    .AmbientLightColour = glm::vec4{ 0.05f, 0.05f, 0.05f, 1.0f },
                    .LightColour        = glm::vec4{ 0.8f, 0.8f, 0.8f, 1.0f } 
                };

                canvas.SetPushConstant(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, &pc, sizeof(pc), 0);

                for (auto& mesh : modelLoader.GetMeshes())
                {
                    canvas.BindBuffer(*mesh.vertexBuffer);
                    canvas.BindBuffer(*mesh.indexBuffer);

                    canvas.DrawIndexed(mesh.indexBuffer->GetCount());
                }
                

                
            }
            

                canvas.EndRendering();
        }

            vkInstance.GetDevice().WaitIdle();
    }
    catch (const std::exception& e)
    {
        RTR::Utility::LogException(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
