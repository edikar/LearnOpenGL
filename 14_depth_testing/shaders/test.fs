#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    FragColor = texture(texture1, TexCoords);


/* 

    //visualizing the depth buffer
    //in the demo - move close to objects to see the effect
 	FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
 */

/* 

 	//Apply linear z buffer rather than inverse 1/z:
 	float near = 0.1; 
 	float far  = 100.0; 
 	float depth = gl_FragCoord.z ;
 	//First we want to transform the depth value to NDC which is not too difficult:
 	float z = depth * 2.0 - 1.0; 
 	//Then take the resulting z value and apply the inverse transformation to retrieve the linear depth value:

 	float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));

 	FragColor = vec4(vec3(linearDepth/ far), 1.0);
 */
}