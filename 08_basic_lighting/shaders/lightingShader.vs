#version 330 core

/* Uniforms */
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

/* In attributes */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

/* Out attributes */
out vec3 FragPos;  
out vec3 Normal;



void main()
{
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //Save the fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    //transform the normals with normal matrix (transpose inverse of model matrix)
    //inverse is a costly operation! it should be done on the CPU and passed as a uniform!!!
    //Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = mat3(normalMat) * aNormal;
}