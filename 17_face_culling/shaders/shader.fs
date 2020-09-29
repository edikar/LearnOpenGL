#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform int front_or_back;

void main()
{
	//if front face - draw regularly
    if(front_or_back == 0)
	    FragColor = texture(texture0, TexCoord);
	else //if back - make it a bit darker... 
		FragColor = vec4(texture(texture0, TexCoord).rgb * 0.5 , 1.0);
}