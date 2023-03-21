/* Start Header ************************************************************************/
/*! \file   ellipsoid.tese
    \author Diren D Bharwani, diren.dbharwani, 390002520 
    \par    diren.dbharwani@digipen.edu
    \date   Nov 25, 2022 
    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
*/ 
/* End Header **************************************************************************/

#version 450

/* Constants ***************************************************************************/ 

const float PI = 3.14159265;

/* Uniforms ****************************************************************************/ 

layout (binding = 1) uniform UBO 
{
  mat4 projection;
  mat4 modelview;
  vec3 abc;
} ubo; 

/* In Attributes ************************************************************************/

patch layout (location = 1)             in vec3 ellipsoidCenter;
      layout(quads, equal_spacing, cw)  in;

/* Out Attributes ***********************************************************************/

      layout (location = 0) out vec3 outNormal;

/* Entry Point **************************************************************************/

void main()
{
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;
  float w = gl_TessCoord.z;
        
  float phi   = PI * ( u - 0.5 );
  float theta = 2.0 * PI * ( v - 0.5 );
  
  vec3 xyz = vec3
  (
    ubo.abc.x * cos(phi) * cos(theta)
  , ubo.abc.y * sin(phi)
  , ubo.abc.z * cos(phi) * sin(theta)
  );

  outNormal = vec3
  (
    cos(phi) * cos(theta)
  , sin(phi)
  , cos(phi) * sin(theta)
  );

  xyz += ellipsoidCenter;
  gl_Position = ubo.projection * ubo.modelview * vec4(xyz, 1.0);
}