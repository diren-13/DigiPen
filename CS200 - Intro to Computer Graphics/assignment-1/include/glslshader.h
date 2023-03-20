/************************************************************************************//*!
\file           glslshader.h
\author         Prasanna Ghali
\par            email: pghali\@digipen.edu
\date           June 13, 2016
\brief          This file contains the declaration of class GLSLShader that encapsulates 
                the functionality required to 
                1.  load shader source
                2.  compile shader source, 
                3.  link shader objects into a program object
                4.  validate program object, 
                5.  log messages from compiling linking
                6.  validation steps
                7.  install program object in
                8.  appropriate shaders
                9.  pass uniform variables from the client to the program object

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>


class GLSLShader
{
  private:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions													               */
    /*---------------------------------------------------------------------------------*/
    enum ShaderType 
    {
        VERTEX_SHADER           = GL_VERTEX_SHADER,
        FRAGMENT_SHADER         = GL_FRAGMENT_SHADER,
        GEOMETRY_SHADER         = GL_GEOMETRY_SHADER,
        TESS_CONTROL_SHADER     = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION_SHADER  = GL_TESS_EVALUATION_SHADER,
        COMPUTE_SHADER          = GL_COMPUTE_SHADER
    };

    /*---------------------------------------------------------------------------------*/
    /* Member Variables													               */
    /*---------------------------------------------------------------------------------*/
    GLuint      pgm_handle  = 0;        // handle to linked shader program object
    GLboolean   is_linked   = GL_FALSE; // has the program successfully linked?
    std::string log_string;             // log for OpenGL compiler and linker messages

  public:
    /*---------------------------------------------------------------------------------*/
    /* Constructors														               */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief     Default constructor for GLSLShader
    *//*********************************************************************************/
    GLSLShader() 
    : pgm_handle(0)
    , is_linked(GL_FALSE) 
    {}

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions															       */
    /*---------------------------------------------------------------------------------*/
    GLuint      GetHandle() const;
    GLboolean   IsLinked()  const;
    std::string GetLog()    const;

    /*---------------------------------------------------------------------------------*/
    /* Setter Functions															       */
    /*---------------------------------------------------------------------------------*/
    void SetUniform(GLchar const* name, GLboolean val);
    void SetUniform(GLchar const* name, GLint val);
    void SetUniform(GLchar const* name, GLfloat val);
    void SetUniform(GLchar const* name, GLfloat x, GLfloat y);
    void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z);
    void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetUniform(GLchar const* name, glm::vec2 const& val);
    void SetUniform(GLchar const* name, glm::vec3 const& val);
    void SetUniform(GLchar const* name, glm::vec4 const& val);
    void SetUniform(GLchar const* name, glm::mat3 const& val);
    void SetUniform(GLchar const* name, glm::mat4 const& val);

    /*---------------------------------------------------------------------------------*/
    /* Member Functions															       */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief      This function not only compiles individual shader sources but links
                multiple shader objects to create an exectuable shader program.

                For each shader source, the function requires the full path to the file
                (containing shader source) and the type of shader program (vertex,
                fragment, geometry, a tessellation type). This information is combined as 
                an std::pair object and the multiple pairs are supplied in a std::vector.

    @param[in]    shaderFiles
        A container of file paths to the shader sources.

    @returns    Status of compiling & linking shaders. True if success, false if fail. 
    *//*********************************************************************************/
    GLboolean CompileLinkValidate(std::vector<std::pair<GLenum, std::string>> shaderFiles);
  
    /********************************************************************************//*!
    @brief      This function does the following:
                1) Create a shader program object if one doesn't exist
                2) Using first parameter, create a shader object
                3) Load shader source code from file specified by second parameter to
                   shader object
                4) Compile shader source by calling CompileShaderFromString
                5) Check compilation status and log any messages to data member 
                   "log_string"
                6) If compilation is successful, attach this shader object to previously
                   created shader program  object

    @param[in]  shader_type
        The type of shader. E.G GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
    @param[in]  file_name
        The file path to the shader source.

    @returns    Status of compiling the shaders. True if success, false if fail.
    *//*********************************************************************************/
    GLboolean CompileShaderFromFile(GLenum shader_type, std::string const& file_name);

    /********************************************************************************//*!
    @brief      This function does the following:
                1) Create a shader program object if one doesn't exist
                2) Using first parameter, create a shader object
                3) Load the shader code from 2nd parameter to shader object
                4) Compile the shader source
                5) Check compilation status and log any messages to data member "log_string"
                6) If compilation is successful, attach this shader object to previously
                   created shader program object

    @param[in]  shader_type
        The type of shader. E.G GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
    @param[in]  shader_src
        The shader source code as a string.

    @returns    Status of compiling the shaders. True if success, false if fail.
    *//*********************************************************************************/
    GLboolean CompileShaderFromString(GLenum shader_type, std::string const& shader_src);

    /********************************************************************************//*!
    @brief      Links shader objects attached to shader handle. If fails to link, writes 
                information log to log_string.

    @returns    Status of linking the shaders. True if success, false if fail.
    *//*********************************************************************************/
    GLboolean Link();

    /********************************************************************************//*!
    @brief      Checks whether the executable shader program object can execute given the
                current OpenGL state. See glValidateProgram() for more info.

    @returns    If validation succeeded. True if success, false if fail.
    *//*********************************************************************************/
    GLboolean Validate();

    /********************************************************************************//*!
    @brief      Use an OpenGL API function to dynamically associate a generic vertex 
                attribute index with a named in attribute variable.

    @param[in]  index
        The binding index for the attribute.
    @param[in]  name
        The name of the attribute.
    *//*********************************************************************************/
    void BindAttribLocation(GLuint index, GLchar const *name);

    /********************************************************************************//*!
    @brief      Use an OpenGL API function to dynamically associate a fragment shader 
                index location that a user-defined out variable will write to.

    @param[in]  color_number
        The colour value.
    @param[in]  name
        The name of the attribute.
    *//*********************************************************************************/
    void BindFragDataLocation(GLuint color_number, GLchar const *name);

    /********************************************************************************//*!
    @brief      Installs the shader program.
    *//*********************************************************************************/
    void Use();

    /********************************************************************************//*!
    @brief      Un-installs the shader program.
    *//*********************************************************************************/
    void UnUse();

    /********************************************************************************//*!
    @brief      Deletes the shader program. See glDeleteProgram() for more info.
    *//*********************************************************************************/
    void DeleteShaderProgram();

    /********************************************************************************//*!
    @brief      Displays the list of active attributes in the shader program to the CLI.
    *//*********************************************************************************/
    void PrintActiveAttribs() const;

    /********************************************************************************//*!
    @brief      Displays the list of active uniforms in the shader program to the CLI.
    *//*********************************************************************************/
    void PrintActiveUniforms() const;

  private:
    /*---------------------------------------------------------------------------------*/
    /* Getter Functions															     */
    /*---------------------------------------------------------------------------------*/
    GLint       GetUniformLocation  (GLchar const *name);
    GLboolean   FileExists          (std::string const& file_name);
};
