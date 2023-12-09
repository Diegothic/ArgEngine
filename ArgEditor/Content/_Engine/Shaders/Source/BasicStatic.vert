#version 460 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexUV;

struct DirLightSpace
{
	mat4 lightSpace;
	mat4 lightSpaceFar;
};

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_Normal;

uniform bool u_ReceiveShadows;
uniform DirLightSpace u_DirLightSpace;

out VS_OUT 
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexUV;

	bool ReceiveShadows;
	vec4 FragPosDirLightSpace;
	vec4 FragPosDirLightSpaceFar;
} vs_out;

void main()
{
	vec4 modelPos = u_Model * vec4(v_Position, 1.0);
	vec4 viewModelPos = u_View * modelPos;
	vs_out.FragPos = vec3(viewModelPos);
	vs_out.Normal = u_Normal * v_Normal;
	vs_out.TexUV = v_TexUV;

	if (u_ReceiveShadows)
	{
		// Light space transforms
		vs_out.FragPosDirLightSpace = u_DirLightSpace.lightSpace * vec4(
			modelPos.xyz,
			1.0
		);
		vs_out.FragPosDirLightSpaceFar = u_DirLightSpace.lightSpaceFar * vec4(
			modelPos.xyz,
			1.0
		);
	}
	vs_out.ReceiveShadows = u_ReceiveShadows;

	// Final vertex position
    gl_Position = u_Proj * viewModelPos;
}
