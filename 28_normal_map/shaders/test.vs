#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    TexCoords = aTexCoords;    

    //transform the normals with normal matrix (transpose inverse of model matrix)
    //inverse is a costly operation! it should be done on the CPU and passed as a uniform!!!
    //Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = mat3(normalMat) * aNormal;

    //save fragment position in the world
    FragPos = vec3(model * vec4(aPos, 1.0));

    //transform T,B,N vectors to world space and construct TBN matrix
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    TBN = mat3(T, B, N);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
