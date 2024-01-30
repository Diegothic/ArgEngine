#version 460 core

layout (location = 0) in vec3 v_Position;

uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	vec4 vertexPosition = vec4(0.0);
    vertexPosition = vec4(v_Position, 1.0);

	vec4 modelPos = u_Model * vertexPosition;
	vec4 viewModelPos = u_View * modelPos;

	// Final vertex position
    gl_Position = u_Proj * viewModelPos;
}
