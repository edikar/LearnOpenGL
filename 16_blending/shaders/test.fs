#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

//in case we want full transparency we discard the current fragment (same as exit() without setting FragColor)
/* void main()
{     
	vec4 texColor = texture(texture1, TexCoords);	
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
} */


//if we use glBlend() then opengl will blend the source and destination according to blending function we set
void main()
{
	FragColor = texture(texture1, TexCoords);
}