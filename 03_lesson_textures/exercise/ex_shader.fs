#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

//exercise4
uniform float mixValue;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    //exersice1
    //FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(-TexCoord.x, TexCoord.y)), 0.2);

    //exersice4
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, vec2(-TexCoord.x, TexCoord.y)), mixValue);


}