#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float horizontatlPos;

out vec3 ourColor; // specify a color output to the fragment shader
out vec4 vPos;

void main()
{
   gl_Position = vec4(aPos.x + horizontatlPos, -aPos.y, aPos.z, 1.0);
   vPos = gl_Position;
   ourColor = aColor;
}