#version 460 core

#define MAX_BONES 100
#define MAX_BONE_WEIGHTS 4

layout (location = 0) in vec3 v_Position;
layout (location = 3) in ivec4 v_BoneIndices;
layout (location = 4) in vec4 v_BoneWeights;

uniform mat4 u_LightSpace;
uniform mat4 u_Model;

uniform bool u_IsSkeletal;
uniform mat4 u_BoneTransforms[MAX_BONES];

void main()
{
    vec4 vertexPosition = vec4(0.0);
    if (u_IsSkeletal)
	{
        for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
        {
            if (v_BoneIndices[i] < 0)
            {
                continue;
            }

            vec4 localPosition = u_BoneTransforms[v_BoneIndices[i]] * vec4(v_Position, 1.0);
            vertexPosition += localPosition * v_BoneWeights[i];
        }
    }
    else
    {
        vertexPosition = vec4(v_Position, 1.0);
    }

    gl_Position = u_LightSpace * u_Model * vertexPosition;
}
