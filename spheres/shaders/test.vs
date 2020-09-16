#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 blackHolePosition;
void main()
{
    TexCoords = aTexCoords;    
    vec4 worldPos =  model * vec4(aPos, 1.0);
    vec3 gravityDirection = blackHolePosition - worldPos.xyz;
   	float distance = length(gravityDirection);
    vec4 newWorldPos = worldPos + vec4(gravityDirection, 1.0) / distance;
    gl_Position = projection * view * newWorldPos;
}