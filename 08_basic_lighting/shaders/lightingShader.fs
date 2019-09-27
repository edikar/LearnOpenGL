#version 330 core

/* Uniforms */
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos; 

/* In attributes */
in vec3 FragPos;  
in vec3 Normal; 

/* Out attributes */
out vec4 FragColor;



void main()
{
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
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColor;  

	//Calculate final color with all the components altogether
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}  