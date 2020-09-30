#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

//if we use glBlend() then opengl will blend the source and destination according to blending function we set
void main()
{
	FragColor = texture(texture1, TexCoords);
}