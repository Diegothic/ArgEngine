#version 460 core

layout (location = 0) in vec3 v_Position;

uniform mat4 u_Proj;
uniform mat4 u_View;

out vec3 TexUV;

void main()
{
    TexUV = v_Position;
    vec4 position = u_Proj * u_View * vec4(v_Position, 1.0);
    gl_Position = position.xyww;
}
