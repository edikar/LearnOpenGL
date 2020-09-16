#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
//    FragColor = vec4(lightColor, 1.0); // set all 4 vector values to 1.0

    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}