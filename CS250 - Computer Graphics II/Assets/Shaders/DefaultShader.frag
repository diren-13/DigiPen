#version 450

#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

// Samplers
layout (binding = 0) uniform sampler2D diffuseSampler;
layout (binding = 1) uniform sampler2D aoSampler;
layout (binding = 2) uniform sampler2D normalSampler;
layout (binding = 3) uniform sampler2D roughnessSampler;

layout(std140, push_constant) uniform PC
{
	mat4 LocalToCamera;
	vec4 LightPos;
	vec4 EyePos;
	vec4 AmbientLightColour;
	vec4 LightColour;
	
} PushConstant;

layout(location = 0) in struct
{
	mat3 BiTangent;
	vec3 LocalPosition;
	vec2 UV;
	
} In;

layout (location = 0) out   vec4 outColour;

void main()
{
	// Normal
    vec3 Normal;
	Normal.xy 	= 	(texture(normalSampler, In.UV).gr * 2.0) - 1.0;
	Normal.z	=	sqrt(1.0 - dot(Normal.xy, Normal.xy));
	Normal 		= 	In.BiTangent * Normal;
	
	// Lighting
	vec3 		LightDirection	= normalize( PushConstant.LightPos.xyz - In.LocalPosition.xyz );	
	float		DiffuseI		= max( 0, dot( Normal, LightDirection ));
	vec3		EyeDirection	= normalize( In.LocalPosition - PushConstant.EyePos.xyz );
	const float Shininess 		= mix( 1, 100, 1 - texture(roughnessSampler, In.UV).r );
	float 		Specular		= pow( max( 0, dot(Normal, normalize( LightDirection - EyeDirection ))), Shininess );
	vec4		DiffuseColour	= texture(diffuseSampler, In.UV);
	
	outColour.rgb	= PushConstant.AmbientLightColour.rgb * DiffuseColour.rgb * texture(aoSampler, In.UV).rgb;
	outColour.rgb  += PushConstant.LightColour.rgb * (Specular.rrr + (DiffuseI.rrr * DiffuseColour.rgb));
	
	const float Gamma			= PushConstant.EyePos.w;
	outColour.a		= DiffuseColour.a;
	outColour.rgb	= pow(outColour.rgb, vec3(1.0f/Gamma));
}