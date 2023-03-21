/* Start Header ************************************************************************/
/*! \file   ellipsoid.frag
    \author Diren D Bharwani, diren.dbharwani, 390002520 
    \par    diren.dbharwani@digipen.edu
    \date   Nov 25, 2022 
    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
*/ 
/* End Header **************************************************************************/

#version 450

/* In Attributes ************************************************************************/

layout (location = 0) in vec3 inNormal;

/* Out Attributes ***********************************************************************/

layout (location = 0) out vec4 outFragColor;

/* Entry Point **************************************************************************/

void main()
{
  vec3 N = normalize(inNormal);
  outFragColor = vec4(inNormal, 1.0);
}