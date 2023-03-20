#version 450

#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(std140, push_constant) uniform PC
{

	mat4 LocalToCamera;
	vec4 LightPos;
	vec4 EyePos;
	vec4 AmbientLightColour;
	vec4 LightColour;
	
} PushConstant;

layout(location = 0) out struct
{
	mat3 BiTangent;
	vec3 LocalPosition;
	vec2 UV;
	
} Out;

void main()
{
	// Decompress the binormal
    vec3 BiNormal		= normalize(cross(inTangent, inNormal));

    // Compute lighting information
    Out.BiTangent		= mat3(inTangent, BiNormal, inNormal);
    Out.LocalPosition	= inPosition;
    Out.UV				= inUV;

    gl_Position			= PushConstant.LocalToCamera * vec4(inPosition.xyz, 1.0);
} 