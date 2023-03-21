/************************************************************************************//*!
\file           glpbo.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 6, 2021
\brief          Contains implementation of functions declared in glpbo.h

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <glpbo.h>

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <iomanip>                          // setprecision

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>	        // glm::pi
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <dpml.h>

#include <glhelper.h>

/*-------------------------------------------------------------------------------------*/
/* Variables for Grader to Modify                                                      */
/*-------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
    If you are adding mode models, make sure to increase the number of models.

    Increasing the number of render modes will not actually do anything but render
    nothing on the screen since no function will be called for additional modes.
---------------------------------------------------------------------------------------*/
static const int            NUM_MODELS          = 2;
static const int            NUM_RENDER_MODE     = 7;

static const std::string    MODEL_FILE_NAME_1   = "ogre.obj";
static const glm::vec3      MODEL_1_SCALE       = { 2.f, 2.f, 2.f };

static const std::string    MODEL_FILE_NAME_2   = "cube.obj";
static const glm::vec3      MODEL_2_SCALE       = { 1.5f, 1.5f, 1.5f };

static const std::string    TEXTURE_FILE_NAME_1 = "ogre.tex";

/*---------------------------------------------------------------------------------------
    Change the camera  & light source parameters here
---------------------------------------------------------------------------------------*/
static const glm::vec3      CAMERA_POS      = { 0.f, 0.f, 10.f };
static const glm::vec3      CAMERA_TARGET   = { 0.f, 0.f, 0.f };

static const glm::dvec3     LIGHT_POS       = { 0.0, 0.0, 10.0 };

/*---------------------------------------------------------------------------------------
    If you do not want left and right parameters to be computed using the formulae
    left = bottom * aspectRatio & right = top * aspectRatio, set this parameter to
    0 and specify appropriate values below.
---------------------------------------------------------------------------------------*/
static const bool COMPUTE_LEFT_AND_RIGHT = 1;

// Six clipping Plane
static const float LEFT    = 1.f;      // if COMPUTE_LEFT_AND_RIGHT = 1, this will be ignored
static const float RIGHT   = 1.f;      // if COMPUTE_LEFT_AND_RIGHT = 1, this will be ignored
static const float TOP     = 1.5f;
static const float BOTTOM  = -1.5f;
static const float NEAR    = 8.f;
static const float FAR     = 12.f;

/*----------------------------------------------------------------------------------
    Change the rotation speed of the model and the light source in the scene here.
    The angle is specified in degrees and represents how much the model will rotate
    in a second about an arbitrary axis.
----------------------------------------------------------------------------------*/
static const float  MODEL_ROTATION_SPEED = glm::radians(60.f);       
static const double LIGHT_ROTATION_SPEED = glm::radians(60.0);

/*-------------------------------------------------------------------------------------*/
/* Static Member Variables                                                             */
/*-------------------------------------------------------------------------------------*/
GLsizei		        GLPbo::width;
GLsizei		        GLPbo::height;
GLsizei		        GLPbo::pixelCount;
GLsizei		        GLPbo::pixelBufferSize;

Colour*		        GLPbo::pPBO         = nullptr;
double*             GLPbo::depthBuffer  = nullptr;
Colour		        GLPbo::clearColour;

GLuint		        GLPbo::PBO;
GLuint		        GLPbo::bufferTexObj;
GLSLShader	        GLPbo::bufferShaderProgram;

glm::mat4           GLPbo::viewTransform;
glm::mat4           GLPbo::orthographicProjection;
glm::mat4           GLPbo::viewportTransform;
glm::mat4           GLPbo::concatViewOrthoViewportTransform;

Model               GLPbo::backgroundQuad;
std::vector<Model>  GLPbo::models;

/*-------------------------------------------------------------------------------------*/
/* Local Variables                                                                     */
/*-------------------------------------------------------------------------------------*/

static Texture      ogreTexture;    // Texture for the ogre model
static PointLight   lightSource;    // Light source for the scene located at the camera's position

static Controller   controller;

static GLboolean&   modelKey        = GLHelper::keystateM;
static GLboolean&   renderModeKey   = GLHelper::keystateW;
static GLboolean&   rotateModelKey  = GLHelper::keystateR;
static GLboolean&   rotateLightKey  = GLHelper::keystateL;
static GLboolean&   xKey            = GLHelper::keystateX;
static GLboolean&   zKey            = GLHelper::keystateZ;

// Tracking variables
static int          culledCount;
static bool         xActive;
static bool         zActive;
static glm::vec3    rotationAxis;

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions															   */
/*-------------------------------------------------------------------------------------*/

// Setup Functions
void GLPbo::SetupPBO()
{
    // Create pbo
    glCreateBuffers(1, &PBO);
    glNamedBufferStorage(PBO, pixelBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

    // Set buffer colour
    pPBO = static_cast<Colour*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
    ClearColourBuffer();
    glUnmapNamedBuffer(PBO);

    // Create Quad
    backgroundQuad.InitWindowSizedQuad();

    // Create texture obj
    glCreateTextures(GL_TEXTURE_2D, 1, &bufferTexObj);
    glTextureStorage2D(bufferTexObj, 1, GL_RGBA8, width, height);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glTextureSubImage2D(bufferTexObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    SetupPBOShader();
}

void GLPbo::SetupPBOShader()
{
    const std::string vertexShaderSource =
    "#version 450 core\n"
    "layout(location = 0)	in  vec2 aVertexPosition;\n"
    "layout(location = 3)	in	vec2 aTextureCoordinate;\n"
    "layout(location = 0)	out vec2 vTextureCoordinate;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
    "	vTextureCoordinate = aTextureCoordinate;\n"
    "}\n";

    const std::string fragmentShaderSource =
    "#version 450 core\n"
    "uniform sampler2D uTex2D;\n"
    "layout(location = 0)	in 	vec2 vTextureCoordinate;\n"
    "layout(location = 0)	out vec4 fFragColour;\n"
    "void main()\n"
    "{\n"
    "	fFragColour = texture(uTex2D, vTextureCoordinate);\n"
    "}\n";

    // Compile-Link-Validate
    bufferShaderProgram.CompileShaderFromString(GL_VERTEX_SHADER, vertexShaderSource);
    bufferShaderProgram.CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource);
    bufferShaderProgram.Link();
    bufferShaderProgram.Validate();

    // Handle failure to link
    if (bufferShaderProgram.IsLinked() == GL_FALSE)
    {
        std::cout << "Unable to compile/link/validate shader programs!" << std::endl;
        std::cout << bufferShaderProgram.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    #ifdef _DEBUG
    std::cout << "Active Shader Attributes\n" << "----------------------------------------------------------------------\n";
    bufferShaderProgram.PrintActiveAttribs();
    std::cout << "\nActive Shader Uniforms\n" << "----------------------------------------------------------------------\n";
    bufferShaderProgram.PrintActiveUniforms();
    std::cout << std::endl;
    #endif

    // Delete shaders
    std::array<GLuint, 2> attachedShaders;
    GLsizei numAttachedShaders;
    glGetAttachedShaders(bufferShaderProgram.GetHandle(), 2, &numAttachedShaders, attachedShaders.data());

    for (auto& shader : attachedShaders) { glDeleteShader(shader); }
}

void GLPbo::SetupMatrices()
{
    const GLsizei   HALF_WIDTH      = width / 2;
    const GLsizei   HALF_HEIGHT     = height / 2;

    const float     ASPECT_RATIO    = static_cast<float>(width) / height;

    const float     DERIVED_LEFT    = ASPECT_RATIO * BOTTOM;
    const float     DERIVED_RIGHT   = ASPECT_RATIO * TOP;

    const glm::vec3 CAM_UP          = { 0.f, 1.f, 0.f };

    // Build Viewport Transform
    viewportTransform =
    {
        HALF_WIDTH, 0.f,            0.f,    0.f,
        0.f,        HALF_HEIGHT,    0.f,    0.f,
        0.f,        0.f,            0.5f,   0.f,
        HALF_WIDTH, HALF_HEIGHT,    0.5f,   1.f
    };

    // Build Orthographic Projection Matrix
    if (COMPUTE_LEFT_AND_RIGHT == true)
    {
        orthographicProjection = glm::ortho(DERIVED_LEFT, DERIVED_RIGHT, BOTTOM, TOP, NEAR, FAR);
    }
    else
    {
        orthographicProjection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
    }
    
    // Build World to Camera Matrix
    viewTransform = glm::lookAt(CAMERA_POS, CAMERA_TARGET, CAM_UP);

    // Concat all 3 for use with ModeltoWorld
    concatViewOrthoViewportTransform = viewportTransform * orthographicProjection * viewTransform;
}

// Setters / Getters
void GLPbo::SetClearColour(Colour colour)
{
    clearColour = colour;
}

void GLPbo::SetPixel(GLint x, GLint y, Colour colour)
{    
    *(pPBO + (y * width + x)) = colour;
}

void GLPbo::SetDepth(GLint x, GLint y, double depth)
{
    *(depthBuffer + (y * width + x)) = depth;
}

double GLPbo::GetDepth(GLint x, GLint y)
{
    return *(depthBuffer + (y * width + x));
}

// Update Functions
void GLPbo::ClearColourBuffer()
{
    std::fill_n(pPBO, pixelCount, clearColour);
}

void GLPbo::ClearDepthBuffer(double value)
{
    std::fill_n(depthBuffer, pixelCount, value);
}

void GLPbo::ViewportTransform(Model& model)
{
    // Build model transform matrix
    model.modelToWorld = glm::rotate(model.rotation, rotationAxis) * glm::scale(model.scale);

    for (auto modelPosIter = model.posVtx.begin(), devicePosIter = model.devicePos.begin(); 
         modelPosIter != model.posVtx.end(); ++modelPosIter, ++devicePosIter)
    {
        // chain transforms
        const glm::mat4 TRANSFORM = concatViewOrthoViewportTransform * model.modelToWorld;
        *devicePosIter = TRANSFORM * glm::vec4(*modelPosIter, 1.f);

        // break loop before device pos goes out of range (redundant check)
        if (devicePosIter == model.devicePos.end()) { break; }
    }
}

void GLPbo::RenderModel(Model& model, Texture& texture, PointLight& lightSource, const int& renderMode)
{
    // Reset this every frame
    culledCount = 0;

    ViewportTransform(model);

    lightSource.worldToModel = glm::inverse(model.modelToWorld);

    const size_t IDX_SIZE = model.idxVtx.size();

    for (size_t idx = 0; idx < IDX_SIZE; idx += Triangle::TRI_PTS)
    {
        glm::dvec3 modelPoints  [Triangle::TRI_PTS]{};
        glm::dvec3 devicePoints [Triangle::TRI_PTS]{};
        glm::dvec3 normals      [Triangle::TRI_PTS]{};
        glm::dvec2 texPoints    [Triangle::TRI_PTS]{};

        for (int i = 0; i < Triangle::TRI_PTS; ++i)
        {
            GLushort elementIdx = model.idxVtx[idx + i];

            modelPoints [i] = model.posVtx      [elementIdx];
            devicePoints[i] = model.devicePos   [elementIdx];
            normals     [i] = model.normalVtx   [elementIdx];
            texPoints   [i] = model.texVtx      [elementIdx];
        }

        TriangleRasteriser tri(modelPoints, devicePoints, normals, texPoints, &texture);
        switch (renderMode)
        {
            case 0:     // Wireframe
            {
                Colour blue(0, 0, 255);

                if (tri.RenderWireframe(blue) == false)
                { 
                    ++culledCount; 
                }
                break;
            }
            case 1:     // Depth Buffer
            {
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::SHADOW_MAP, lightSource) == false) 
                { 
                    ++culledCount; 
                }
                break;
            }
            case 2:     // Faceted Shading
            {              
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::FACETED, lightSource) == false)
                {
                    ++culledCount;
                }
                break;
            }
            case 3:     // Smooth Shading
            {
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::SMOOTH, lightSource) == false)
                {
                    ++culledCount;
                }
                break;
            }
            case 4:     // Texture
            {
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::SHADOW_MAP, lightSource, true) == false)
                {
                    ++culledCount;
                }
                break;
            }
            case 5:     // Texture + Faceted Shading
            {
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::FACETED, lightSource, true) == false)
                {
                    ++culledCount;
                }
                break;
            }
            case 6:     // Texture + Smooth Shading
            {
                if (tri.RenderShadedTriangle(TriangleRasteriser::RM::SMOOTH, lightSource, true) == false)
                {
                    ++culledCount;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions															   */
/*-------------------------------------------------------------------------------------*/
void GLPbo::emulate()
{
    // Update states in the scene
    controller.Update(modelKey, renderModeKey, rotateModelKey, rotateLightKey);

    // Update rotation Axis
    if (xKey == GL_TRUE)
    {
        xActive = !xActive;
        rotationAxis.x = xActive ? 1.f : 0.f;
        xKey = GL_FALSE;
    }
    if (zKey == GL_TRUE)
    {
        zActive = !zActive;
        rotationAxis.z = zActive ? 1.f : 0.f;
        zKey = GL_FALSE;
    }

    // Render Model
    Model& currentModel = models[controller.model];

    // Update rotation
    if (controller.rotateModel == true) { currentModel.rotation += MODEL_ROTATION_SPEED * static_cast<float>(GLHelper::dt); }
    if (controller.rotateLight == true) { lightSource.rotation  += LIGHT_ROTATION_SPEED * GLHelper::dt; }

    pPBO = static_cast<Colour*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
    ClearColourBuffer();
    ClearDepthBuffer(1.0);
    RenderModel(currentModel, ogreTexture, lightSource, controller.renderMode);
    glUnmapNamedBuffer(PBO);

    glTextureSubImage2D(bufferTexObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    SetTitleBar();
}

void GLPbo::SetTitleBar()
{
    std::stringstream sstr;
    std::stringstream trailingSStr;

    // Title
    sstr << "CSD2100";

    // -- FPS
    sstr << std::fixed << std::setprecision(2) << " | FPS: " << GLHelper::fps;

    Model& currentModel = models[controller.model];

    // Num Vertex
    sstr << " | Vertices: ";
    sstr << currentModel.posVtx.size();

    // Num Triangles
    sstr << " | Triangles: ";
    sstr << currentModel.primitiveCount;

    // Num Culled
    sstr << " | Culled: ";
    sstr << culledCount << " |";

    // Render Mode
    switch (controller.renderMode)
    {
        case 0:     // Wireframe
        {
            sstr << " Wireframe";
            break;
        }
        case 1:     // Depth Buffer
        {
            sstr << " Depth Buffer";
            break;
        }
        case 2:     // Faceted
        {
            sstr << " Faceted";
            break;
        }
        case 3:     // Shaded
        {
            sstr << " Shaded";
            break;
        }
        case 4:     // Texture
        {
            sstr << " Textured";
            break;
        }
        case 5:     // Texture + Faceted
        {
            sstr << " Textured + Faceted";
            break;
        }
        case 6:     // Texture + Faceted
        {
            sstr << " Textured + Shaded";
            break;
        }
        default:
        {
            sstr << " INVALID!";
            break;
        }
    }

    glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());
}

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions																   */
/*-------------------------------------------------------------------------------------*/

void GLPbo::init(GLsizei w, GLsizei h)
{
    // Assign values
    width           = w;
    height          = h;
    pixelCount      = width * height;
    pixelBufferSize = sizeof(Colour) * pixelCount;

    // Initial Setup
    SetupMatrices();
    SetClearColour({ 0, 0, 0 });
    SetupPBO();

    // Allocate memory for depth buffer
    depthBuffer = new double[pixelCount];
    
    // Setup Texture
    std::cout << "Loading Textures...\n" << "--------------------------------------------------------" << std::endl;
    ogreTexture.Init(TEXTURE_FILE_NAME_1);
    std::cout << "--------------------------------------------------------\n" << std::endl;

    // Setup Models
    models.resize(NUM_MODELS);
    std::cout << "Loading Models...\n" << "--------------------------------------------------------" << std::endl;
    models[0].InitModel(MODEL_FILE_NAME_1);
    models[0].scale = MODEL_1_SCALE;
    models[1].InitModel(MODEL_FILE_NAME_2);
    models[1].scale = MODEL_2_SCALE;
    std::cout << "--------------------------------------------------------" << std::endl;

    // Setup Light Source(s)
    lightSource.position = LIGHT_POS;

    // Setup Controller
    controller.SetNumModels(NUM_MODELS);
    controller.SetNumModes(NUM_RENDER_MODE);

    // Init tracking variables
    culledCount         = 0;
    xActive = zActive   = false;
    rotationAxis        = { 0.f, 1.f, 0.f };

    #ifdef _DEBUG
    // Print current GPU info to CLI
    GLHelper::print_specs();
    #endif
}

void GLPbo::draw_fullwindow_quad()
{
    glBindTextureUnit(0, bufferTexObj);

    bufferShaderProgram.Use();
    bufferShaderProgram.SetUniform("uTex2D", 0);

    glBindVertexArray(backgroundQuad.VAO);
    glDrawElements(backgroundQuad.primitiveType, backgroundQuad.drawCount, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);

    bufferShaderProgram.UnUse();
}

void GLPbo::cleanup()
{
    // Delete buffers & VAO
    backgroundQuad.Release();

    glDeleteBuffers(1, &PBO);
    delete[] depthBuffer;

    // Delete texture objects
    glDeleteTextures(1, &bufferTexObj);

    // Delete shader programs
    bufferShaderProgram.DeleteShaderProgram();
}
