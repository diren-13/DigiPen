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

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>	// glm::pi

#include <glhelper.h>

/*-------------------------------------------------------------------------------------*/
/* Static Member Variables                                                             */
/*-------------------------------------------------------------------------------------*/
GLsizei GLPbo::width;
GLsizei GLPbo::height;
GLsizei GLPbo::pixelCount;
GLsizei GLPbo::byteCount;

GLPbo::Color* GLPbo::pPBO = nullptr;
GLPbo::Color  GLPbo::clearColour;

GLuint GLPbo::VAO;
GLuint GLPbo::VBO;
GLuint GLPbo::EBO;
GLuint GLPbo::texObj;
GLuint GLPbo::PBO;
GLuint GLPbo::elementCount;
GLSLShader GLPbo::shaderProgram;

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/
GLPbo::Color::Color(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)\
: r	(_r)
, g	(_g)
, b	(_b)
, a	(_a)
{}

/*-------------------------------------------------------------------------------------*/
/* Member Functions																	   */
/*-------------------------------------------------------------------------------------*/
void GLPbo::setup_quad_vao()
{
	std::array<glm::vec2, 4>	posVtx, texVtx;
	std::array<GLushort, 4>		idxVtx;

	// Populate arrays
	posVtx =
	{
		glm::vec2{ -1.f, -1.f}, glm::vec2{ 1.f, -1.f},
		glm::vec2{ -1.f,  1.f}, glm::vec2{ 1.f,  1.f}
	};

	texVtx =
	{
		glm::vec2{ 0.f, 0.f}, glm::vec2{ 1.f, 0.f},
		glm::vec2{ 0.f, 1.f}, glm::vec2{ 1.f, 1.f}
	};

	idxVtx = { 2, 0, 3, 1 };

	// Send data to GPU
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	const GLsizei POS_SIZE = sizeof(glm::vec2) * posVtx.size();
	const GLsizei TEX_SIZE = sizeof(glm::vec2) * texVtx.size();
	const GLsizei IDX_SIZE = sizeof(GLushort) * idxVtx.size();

	glNamedBufferStorage(VBO, POS_SIZE + TEX_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(VBO, 0, POS_SIZE, posVtx.data());
	glNamedBufferSubData(VBO, POS_SIZE, TEX_SIZE, texVtx.data());
	
	glNamedBufferStorage(EBO, IDX_SIZE, idxVtx.data(), GL_DYNAMIC_STORAGE_BIT);

	// Set up VAO
	glCreateVertexArrays(1, &VAO);

	// -- attribute & binding point indexes
	const GLuint POS_ATTRIB		= 0;
	const GLuint POS_BINDING_PT = 0;
	const GLuint TEX_ATTRIB		= 2;
	const GLuint TEX_BINDING_PT = 2;

	// -- Position
	glEnableVertexArrayAttrib(VAO, POS_ATTRIB);
	glVertexArrayVertexBuffer(VAO, POS_BINDING_PT, VBO, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(VAO, POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, POS_ATTRIB, POS_BINDING_PT);
	// -- Texture
	glEnableVertexArrayAttrib(VAO, TEX_ATTRIB);
	glVertexArrayVertexBuffer(VAO, TEX_BINDING_PT, VBO, POS_SIZE, sizeof(glm::vec2));
	glVertexArrayAttribFormat(VAO, TEX_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, TEX_ATTRIB, TEX_BINDING_PT);
	// -- EBO
	glVertexArrayElementBuffer(VAO, EBO);
	
	// -- Unbind VAO to prevent further changes
	glBindVertexArray(0);

	elementCount = idxVtx.size();
}

void GLPbo::setup_shdrpgm()
{
	std::string vertexShaderSource =
	"#version 450 core\n"
	"layout(location = 0)	in  vec2 aVertexPosition;\n"
	"layout(location = 2)	in	vec2 aTextureCoordinate;\n"
	"layout(location = 0)	out vec2 vTextureCoordinate;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
	"	vTextureCoordinate = aTextureCoordinate;\n"
	"}\n";

	std::string fragmentShaderSource =
	"#version 450 core\n"
	"uniform sampler2D uTex2D;\n"
	"layout(location = 0)	in 	vec2 vTextureCoordinate;\n"
	"layout(location = 0)	out vec4 fFragColour;\n"
	"void main()\n"
	"{\n"
	"	fFragColour = texture(uTex2D, vTextureCoordinate);\n"
	"}\n";

	// Compile-Link-Validate
	shaderProgram.CompileShaderFromString(GL_VERTEX_SHADER, vertexShaderSource);
	shaderProgram.CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource);
	shaderProgram.Link();
	shaderProgram.Validate();

	// Handle failure to link
	if (shaderProgram.IsLinked() == GL_FALSE)
	{
		std::cout << "Unable to compile/link/validate shader programs!" << std::endl;
		std::cout << shaderProgram.GetLog() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	shaderProgram.PrintActiveAttribs();
	shaderProgram.PrintActiveUniforms();

	// Delete shaders
	std::array<GLuint, 2> attachedShaders;
	GLsizei numAttachedShaders;
	glGetAttachedShaders(shaderProgram.GetHandle(), 2, &numAttachedShaders, attachedShaders.data());

	for (auto& shader : attachedShaders) { glDeleteShader(shader); }
}

void GLPbo::set_clear_color(Color colour)
{
	clearColour = colour;
}

void GLPbo::set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	set_clear_color({ r, g, b, a });
}

void GLPbo::clear_color_buffer()
{
	std::fill_n(pPBO, pixelCount, clearColour);
}

void GLPbo::emulate()
{
	// interpolate green and blue colours
	double greenValue = (sin(glfwGetTime()) + 1) * (255 * 0.5);
	double blueValue  = (cos(glfwGetTime()) + 1) * (255 * 0.5);

	set_clear_color(255, static_cast<GLubyte>(greenValue), static_cast<GLubyte>(blueValue));

	pPBO = static_cast<Color*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
	clear_color_buffer();
	glUnmapNamedBuffer(PBO);

	glTextureSubImage2D(texObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions																   */
/*-------------------------------------------------------------------------------------*/

void GLPbo::init(GLsizei w, GLsizei h)
{
	// Assign values
	width		= w;
	height		= h;
	pixelCount	= width * height;
	byteCount	= sizeof(Color) * pixelCount;

	// Init default colour buffer
	set_clear_color(255, 255, 255);

	// Create pbo
	glCreateBuffers(1, &PBO);
	glNamedBufferStorage(PBO, byteCount, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	// Set buffer colour
	pPBO = static_cast<Color*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
	clear_color_buffer();
	glUnmapNamedBuffer(PBO);

	setup_quad_vao();

	// Create texture obj
	glCreateTextures(GL_TEXTURE_2D, 1, &texObj);
	glTextureStorage2D(texObj, 1, GL_RGBA8, width, height);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
	glTextureSubImage2D(texObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	setup_shdrpgm();

	// Print current GPU info to CLI
	GLHelper::printGPUSpecs();
}

void GLPbo::draw_fullwindow_quad()
{
	glBindTextureUnit(0, texObj);

	shaderProgram.Use();
	shaderProgram.SetUniform("uTex2D", 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, elementCount, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);

	shaderProgram.UnUse();
}

void GLPbo::cleanup()
{
	// Delete buffers & VAO
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &PBO);
	glDeleteVertexArrays(1, &VAO);

	// Delete texture objects
	glDeleteTextures(1, &texObj);

	// Delete shader programs
	shaderProgram.DeleteShaderProgram();
}