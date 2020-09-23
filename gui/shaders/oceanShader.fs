#version 330 core

#extension GL_OES_standard_derivatives : enable

/* Uniforms */
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform bool useHeightColor;

/* In attributes */
in vec3 FragPos;  
in vec3 Normal; 

/* 
in vec2 TexCoords;
uniform sampler2D texture1;
 */
/* Out attributes */
out vec4 FragColor;

void main()
{
	//vec3 Normal = normalize( 1 * normalize( cross( dFdx( FragPos.xyz ), dFdy( FragPos.xyz ) ) ) );
	
	//float phi = atan(Normal.z/Normal.x);
	//float theta = acos(Normal.y);
	//vec3 Normal = normalize(Normal * texture(texture1, vec2(phi, theta)).rgb);

	//Calculate ambient light component
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    //Calculate light direction and normal
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos); 
	//Calculate the diffuse light component
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//calculate the specular light component
	float specularStrength = 5.0;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColor;  


    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 0.85);
}  