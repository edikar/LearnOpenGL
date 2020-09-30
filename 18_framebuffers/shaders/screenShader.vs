#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    // note that we read the multiplication from right to left
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	TexCoord = aTexCoord;
}