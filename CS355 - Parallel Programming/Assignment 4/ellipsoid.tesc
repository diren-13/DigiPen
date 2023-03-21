/* Start Header ************************************************************************/
/*! \file   ellipsoid.tesc
    \author Diren D Bharwani, diren.dbharwani, 390002520 
    \par    diren.dbharwani@digipen.edu
    \date   Nov 25, 2022 
    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
*/ 
/* End Header **************************************************************************/

#version 450

/* Uniforms ****************************************************************************/ 

layout (binding = 0) uniform UBO 
{
  vec3 center;
  float tessLevel;
} ubo; 
 
/* Out Attributes ***********************************************************************/

      layout (vertices = 1) out;
patch layout (location = 1) out vec3 ellipsoidCenter;
 
/* Entry Point **************************************************************************/

void main()
{
  gl_TessLevelOuter[0] = 2.;
  gl_TessLevelOuter[1] = ubo.tessLevel;
  gl_TessLevelOuter[2] = 2.;
  gl_TessLevelOuter[3] = ubo.tessLevel;
  gl_TessLevelInner[0] = ubo.tessLevel;
  gl_TessLevelInner[1] = ubo.tessLevel;

  ellipsoidCenter = ubo.center;
} 
