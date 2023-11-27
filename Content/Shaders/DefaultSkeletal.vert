#version 460 core

#define MAX_BONES 100
#define MAX_BONE_WEIGHTS 4

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexUV;
layout (location = 3) in ivec4 v_BoneIndices;
layout (location = 4) in vec4 v_BoneWeights;

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

uniform mat4 u_BoneTransforms[MAX_BONES];

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
	vec4 vertexPosition = vec4(0.0);
	vec3 vertexNormal = vec3(0.0);
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		if (v_BoneIndices[i] < 0)
		{
			continue;
		}

		vec4 localPosition = u_BoneTransforms[v_BoneIndices[i]] * vec4(v_Position, 1.0);
		vertexPosition += localPosition * v_BoneWeights[i];

		vec3 localNormal = mat3(u_BoneTransforms[v_BoneIndices[i]]) * v_Normal;
		vertexNormal += localNormal * v_BoneWeights[i];
	}

	vec4 modelPos = u_Model * vertexPosition;
	vec4 viewModelPos = u_View * modelPos;
	vs_out.FragPos = vec3(viewModelPos);
	vs_out.Normal = u_Normal * vertexNormal;
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
