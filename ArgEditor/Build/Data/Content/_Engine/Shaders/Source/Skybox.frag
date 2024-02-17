#version 460 core

uniform samplerCube u_SkyBox;

in vec3 TexUV;

out vec4 FragColor;

void main()
{
   FragColor = texture(u_SkyBox, TexUV);
}
