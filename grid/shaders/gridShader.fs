#version 330 core

/* Uniforms */
uniform vec3 playerPos;

/* In attributes */
in vec3 Normal; 

/* Out attributes */
out vec4 FragColor;

void main()
{
    FragColor = vec4(playerPos.x /1000, playerPos.y /1000, 1.0, 1.0);
}  