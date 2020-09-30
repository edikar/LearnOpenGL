#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;

void main()
{
	//improved grayscale effect
	FragColor = texture(texture0, TexCoord);
	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);

	//grayscale effect
	//FragColor = texture(texture0, TexCoord);
	//float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
	//FragColor = vec4(average, average, average, 1.0);
 

 	//inverse colors effect
	//FragColor = vec4(vec3(1.0 - texture(texture0, TexCoord)), 1.0);
 

	//regular
	//FragColor = texture(texture0, TexCoord);

}