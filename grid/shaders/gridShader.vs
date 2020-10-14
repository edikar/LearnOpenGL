#version 330 core

/* Uniforms */
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 playerPos;

/* In attributes */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

/* Out attributes */
out vec3 FragPos;  
out vec3 Normal;

void main()
{
    vec2 playerPos2D = playerPos.xy;
    float dist = length(aPos.xy - playerPos2D);
    vec3 vPos = aPos;
    //if(dist > 300)
    //    vPos = aPos + (100/dist) * vec3(playerPos2D - aPos.xy, 0.0);

    float warpPercent = dist/1000;
    vPos = aPos + warpPercent * vec3(playerPos2D - aPos.xy, 0.0);
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}














