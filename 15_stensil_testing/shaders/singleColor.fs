#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    FragColor = vec4(0.9, 0.3, 0.3, 1.0);
}